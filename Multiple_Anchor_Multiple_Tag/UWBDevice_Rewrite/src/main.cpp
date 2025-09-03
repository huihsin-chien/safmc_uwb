#include "uwb_common.hpp"
#include "config.hpp"
#include <ESP.h>
#include <map>
#include <deque>
#include <utility>
#include <vector>
#include <iostream>
#include <cmath>
#include <limits>
#include <functional>
#include <ArduinoEigen.h>

using namespace Eigen;
using namespace std;



typedef std::pair<float, unsigned long> RangeTimePair;
typedef std::deque<RangeTimePair> RangeQueue;
std::map<uint16_t, RangeQueue> tagRangeQueues;
std::map<uint16_t, double> averageDistanceMap; // key: tag address, value: distance



bool isAnchor = false;

double a = 0.0; // 地上兩個tag的位置

unsigned long lastSerialOutput = 0;
unsigned long lastCheck = 0;
int serialOutputCount = 0;

void as_tag();
void as_anchor();

void setup() {
    Serial.println("### DW1000Ng-arduino-ranging-tag2 ###");
    isAnchor = isAnchorByDefault;
    Serial.begin(9600);
    setupUWB(&EUI[0], self_device_address, isAnchor ? ANCHOR_FRAME_FILTER_CONFIG : TAG_FRAME_FILTER_CONFIG); // 2 is the device address of the anchorB
}

void try_to_change_state() {
    // 先記錄新狀態而不轉換，直到 Serial 空了時再轉換，以減少頻繁的 enableFrameFiltering()
    bool newIsAnchor = isAnchor;

    // 讀取狀態轉換指令
    char ch = '\0';
    while(Serial.available()){
        char newCh = Serial.read();

        // 僅在連續輸入相同字元時才判斷，以避免雜訊
        if(newCh != ch) {
            ch = newCh;
            continue;
        }

        // 依照指令設定新狀態
        // 是 anchor 就判斷轉換成 tag，是 tag 就判斷轉換成 anchor
        if(newIsAnchor) {
            for(int i = 0; becomeTagSymbols[i] != '\0'; i++){
                if(ch == becomeTagSymbols[i])
                    newIsAnchor = false;
            }
        } else {
            for(int i = 0; becomeAnchorSymbols[i] != '\0'; i++){
                if(ch == becomeAnchorSymbols[i])
                    newIsAnchor = true;
            }
        }
    } // end of while(Serial.available())

    // 如果狀態不變，就提早離開
    if(newIsAnchor == isAnchor) 
        return;

    isAnchor = newIsAnchor;
    if(isAnchor)
        DW1000Ng::enableFrameFiltering(TAG_FRAME_FILTER_CONFIG);
    else
        DW1000Ng::enableFrameFiltering(ANCHOR_FRAME_FILTER_CONFIG);
}

void loop() {
    if(Serial.available() > 0)
        try_to_change_state();

    try{
        if(isAnchor) {
            as_anchor();
        } else {
            as_tag();
        }
    } catch (const std::exception& e) {
    }

    if(AUTO_RESTART){
        // 每10秒檢查一次
        if(millis() - lastCheck > 10000) {
            lastCheck = millis();
            if(serialOutputCount < 3) { // 10秒內少於3次輸出就當死掉
                ESP.restart();
            }
            serialOutputCount = 0; // reset counter
        }
    }
}


// 回傳新的 blink_rate
void as_tag() {
    // Serial.println("let's go~");
    // DW1000Ng::deepSleep();
    delay(blink_rate);
    // DW1000Ng::spiWakeup();

    for (uint16_t target_anchor = 1; target_anchor <= 8; target_anchor++) {
        RangeResult result = DW1000NgRTLS_ext::tagFinishRange(target_anchor, 1500);
        if(result.success) {
            // Serial.println("result is right!");
            #ifndef FIXED_BLINK_RATE
            blink_rate = result.new_blink_rate;
            #endif
        } else {
            // 未成功的話，印出失敗訊息。格式為：`tag_range,${tag_eui},failed,${anchorDeviceAddress}`
            Serial.print("tag_range,"); Serial.print(&EUI[18]); 
            Serial.print(",failed,"); Serial.println(target_anchor);
        }
    }
}


Vector2d calculateXY(double a, double b, double c) {
    double x=(a*a-c*c+b*b)/(2*a);
    double y=sqrt(abs(b*b-x*x));
    Vector2d result(x, y);
    return result;
}
//                  /\ 
//               b /  \ c
//                /    \ 
//      tag 0101 -------- tag 0202
//                   a


double errorFunction(const Vector2d& x,
                     const vector<Vector2d>& anchors,
                     const vector<double>& distances)
{
    double error = 0.0;
    for (size_t i = 0; i < anchors.size(); ++i) {
        double dist = (x - anchors[i]).norm();
        error += pow(dist - distances[i], 2);
    }
    return error;
}

Vector2d gpsSolve(const vector<Vector2d>& anchors,
                  const vector<double>& distances,
                  const Vector2d& initial_guess,
                  double tol = 1e-4,
                  int max_iter = 1000)
{
    // 使用梯度下降作為優化器 (TODO: 實作 Nelder-Mead）
    Vector2d x = initial_guess;
    double alpha = 0.01; // 學習率

    for (int iter = 0; iter < max_iter; ++iter) {
        Vector2d grad = Vector2d::Zero();

        // 數值估計梯度
        double h = 1e-6;
        for (int d = 0; d < 2; ++d) {
            Vector2d x1 = x;
            x1[d] += h;
            Vector2d x2 = x;
            x2[d] -= h;
            grad[d] = (errorFunction(x1, anchors, distances) - errorFunction(x2, anchors, distances)) / (2 * h);
        }

        Vector2d x_new = x - alpha * grad;

        if ((x_new - x).norm() < tol)
            break;

        x = x_new;
    }

    return x;
}

Vector2d previous_position_result(0, 4);
void as_anchor(){
    // 取得 tagFinishRange() 傳出的封包，並回傳接受
    RangeAcceptResult result = DW1000NgRTLS_ext::anchorRangeAccept(NextActivity::ACTIVITY_FINISHED, blink_rate);
    if(!result.success) 
        return;
    
    delay(2); // Tweak based on your hardware

    // 取得 tagFinishRange() 呼叫的 transmitPoll() 傳出的封包的資料
    size_t recv_len = DW1000Ng::getReceivedDataLength();
    byte recv_data[recv_len];
    DW1000Ng::getReceivedData(recv_data, recv_len);

    // 如果計算結果明顯不合理，則提早進入下一個迴圈
    if(result.range < 0.001 || result.range > 500) 
        return;


    // 印出距離訊息
    char rangeNumString[32];
    snprintf(rangeNumString, 32, "%lf", result.range);
    String rangeString = "Range: "; rangeString += rangeNumString; rangeString += " m";
    rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm distance between anchor/tag:";
    rangeString += recv_data[7]; rangeString += recv_data[8]; // tag 的短 EUI
    rangeString += " from Anchor "; rangeString += EUI[18]; rangeString += EUI[19]; rangeString += EUI[20]; rangeString += EUI[21]; rangeString += EUI[22]; rangeString += EUI[23];
    Serial.println(rangeString);

    // 印出距離訊息。格式為：`anchor_range,${distance},${tagEUI},${anchorEUI}`
    char rangeCharArr[256];
    snprintf(rangeCharArr, 256, "anchor_range,%lf,%02x:%02x,%s", result.range, recv_data[8], recv_data[7], &EUI[18]);
    Serial.println(rangeCharArr);

    // 記錄 serial 輸出
    lastSerialOutput = millis();
    serialOutputCount++;


    //7/8 edit
    // 將距離和時間的pair存入tagRangeQueues

    uint16_t tag_id = (recv_data[8] << 8) | recv_data[7];
    unsigned long now = millis();
    tagRangeQueues[tag_id].emplace_back(result.range, now);

    //如果超過1.5秒的訊息就刪掉
    RangeQueue& q = tagRangeQueues[tag_id];

    Serial.print("Tag ID: ");
    Serial.print(tag_id, HEX);
    while (!q.empty() && q.front().second + 1500 < now) {
        q.pop_front();
        // Serial.print(q.front().first);
    }
    
    double total =0;
   
    for (const auto& data : q) {
        total += data.first;
    }
    double average = q.empty() ? 0 : total / q.size();
    //Average range for tag 0101: 2.5 m
    Serial.print("Average range for tag "); 
    Serial.print(tag_id, HEX);

    // TODO: 測量線性修正固定偏差值 & 縮放比例(0-60m)
    // 0813 測量結果
    average = 0.9964 * average - 0.6446;
    Serial.print(": "); 
    Serial.print(average);
    Serial.println(" m");

    // 計算 x 和 y
    averageDistanceMap[tag_id] = average; 

    if (!useGpsSolve){
        calculateXY(1.3, averageDistanceMap[0x0101], averageDistanceMap[0x0202]); // 假設 a 是 8.0，tag_id 是 0x0101

    }
    else {
        try{

            vector<Vector2d> available_anchors;
            vector<double> available_distances;

            if (averageDistanceMap.find(0x0101) != averageDistanceMap.end()) {
                available_anchors.push_back(Vector2d(0, 0));  // Anchor 1 position
                available_distances.push_back(averageDistanceMap[0x0101]);
                // Serial.println("Using anchor 0x0101");
            }

            if (averageDistanceMap.find(0x0202) != averageDistanceMap.end()) {
                available_anchors.push_back(Vector2d(8, 0));  // Anchor 2 position  
                available_distances.push_back(averageDistanceMap[0x0202]);
                // Serial.println("Using anchor 0x0202");
            }

            if (averageDistanceMap.find(0x0303) != averageDistanceMap.end()) {
                available_anchors.push_back(Vector2d(4, -5)); // Anchor 3 position
                available_distances.push_back(averageDistanceMap[0x0303]);
                // Serial.println("Using anchor 0x0303");
            }

            // Only proceed if we have at least 2 anchors
            if (available_anchors.size() >= 2) {
                Vector2d position_result = gpsSolve(available_anchors, available_distances, calculateXY( 8, available_distances[0], available_distances[1]));
                
                Serial.print("Estimated position: (");
                Serial.print(position_result.x());
                Serial.print(", ");
                Serial.print(position_result.y());
                Serial.println(")");
            }
        }
        catch (const std::exception& e) {
            Serial.print("Error in GPS calculation: ");
        }
    }


    }
    
