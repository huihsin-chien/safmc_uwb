// /*
//  * MIT License
//  * 
//  * Copyright (c) 2018 Michele Biondi, Andrea Salvatori
//  * 
//  * Permission is hereby granted, free of charge, to any person obtaining a copy
//  * of this software and associated documentation files (the "Software"), to deal
//  * in the Software without restriction, including without limitation the rights
//  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  * copies of the Software, and to permit persons to whom the Software is
//  * furnished to do so, subject to the following conditions:
//  * 
//  * The above copyright notice and this permission notice shall be included in all
//  * copies or substantial portions of the Software.
//  * 
//  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  * SOFTWARE.
// */

// /*
//  * Copyright (c) 2015 by Thomas Trojer <thomas@trojer.net>
//  * Decawave DW1000Ng library for arduino.
//  *
//  * Licensed under the Apache License, Version 2.0 (the "License");
//  * you may not use this file except in compliance with the License.
//  * You may obtain a copy of the License at
//  *
//  *     http://www.apache.org/licenses/LICENSE-2.0
//  *
//  * Unless required by applicable law or agreed to in writing, software
//  * distributed under the License is distributed on an "AS IS" BASIS,
//  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  * See the License for the specific language governing permissions and
//  * limitations under the License.
//  *
//  * @file BasicSender.ino
//  * Use this to test simple sender/receiver functionality with two
//  * DW1000Ng:: Complements the "BasicReceiver" example sketch. 
//  * 
//  * @todo
//  *  - move strings to flash (less RAM consumption)
//  *  
//  */

#include <Arduino.h>
#include <DW1000Ng.hpp>
#include <DW1000NgUtils.hpp>
#include <DW1000NgTime.hpp>
#include <DW1000NgConstants.hpp>
// // #if defined(ESP8266)
// // //const uint8_t PIN_RST = 5; // reset pin
// // //const uint8_t PIN_IRQ = 4; // irq pin
// // const uint8_t PIN_SS = 15; // spi select pin
// // #else
const uint8_t PIN_RST = 15; // reset pin
const uint8_t PIN_IRQ = 16; // irq pin
const uint8_t PIN_SS = 10; // spi select pin
// // #endif

#define POLL 0
#define POLL_ACK 1
#define RANGE 2
#define RANGE_REPORT 3
#define RANGE_FAILED 255
// message flow state
volatile byte expectedMsgId = POLL_ACK;
// message sent/received state
volatile boolean sentAck = false;
volatile boolean receivedAck = false;
// timestamps to remember
uint64_t timePollSent;
uint64_t timePollAckReceived;
uint64_t timeRangeSent;
// data buffer
#define LEN_DATA 16
byte data[LEN_DATA];
// watchdog and reset period
uint32_t lastActivity;
uint32_t resetPeriod = 2;
// reply times (same on both sides for symm. ranging)
uint16_t replyDelayTimeUS = 3000;

device_configuration_t DEFAULT_CONFIG = {
    false,
    true,
    true,
    true,
    false,
    SFDMode::STANDARD_SFD,
    Channel::CHANNEL_5,
    DataRate::RATE_850KBPS,
    PulseFrequency::FREQ_16MHZ,
    PreambleLength::LEN_256,
    PreambleCode::CODE_3
};

interrupt_configuration_t DEFAULT_INTERRUPT_CONFIG = {
    true,
    true,
    true,
    false,
    true
};

void handleSent();
void handleReceived();
void transmitPoll();
void noteActivity();

void setup() {
    // DEBUG monitoring
    delay(5000);
    Serial.begin(9600);
    Serial.println(F("### DW1000Ng-arduino-ranging-tag ###"));
    // initialize the driver
    DW1000Ng::initialize(PIN_SS, PIN_IRQ , PIN_RST);
    Serial.println("DW1000Ng initialized ...");
    // general configuration
    DW1000Ng::applyConfiguration(DEFAULT_CONFIG);
	DW1000Ng::applyInterruptConfiguration(DEFAULT_INTERRUPT_CONFIG);

    DW1000Ng::setDeviceAddress(5);  //tag    // 設定 Device ID
    DW1000Ng::setNetworkId(10);     // 設定 PAN ID         
    
    DW1000Ng::setAntennaDelay(16436);
    
    Serial.println(F("Committed configuration ..."));
    // DEBUG chip info and registers pretty printed
    char msg[128];
    DW1000Ng::getPrintableDeviceIdentifier(msg);
    Serial.print("Device ID: "); Serial.println(msg);
    DW1000Ng::getPrintableExtendedUniqueIdentifier(msg);
    Serial.print("Unique ID: "); Serial.println(msg);
    DW1000Ng::getPrintableNetworkIdAndShortAddress(msg);
    Serial.print("Network ID & Device Address: "); Serial.println(msg);
    DW1000Ng::getPrintableDeviceMode(msg);
    Serial.print("Device mode: "); Serial.println(msg);
    // attach callback for (successfully) sent and received messages
    DW1000Ng::attachSentHandler(handleSent);
    DW1000Ng::attachReceivedHandler(handleReceived);
    // anchor starts by transmitting a POLL message

    DW1000Ng::enableDebounceClock();
    DW1000Ng::enableLedBlinking();
    DW1000Ng::setGPIOMode(6, LED_MODE);
    DW1000Ng::setGPIOMode(8, LED_MODE);
    DW1000Ng::setGPIOMode(10, LED_MODE);
    DW1000Ng::setGPIOMode(12, LED_MODE);

    transmitPoll();
    noteActivity();
    Serial.println("setup end");
    delay(2000) ;
}

void noteActivity() {
    // update activity timestamp, so that we do not reach "resetPeriod"
    lastActivity = millis();
}

void resetInactive() {
    // tag sends POLL and listens for POLL_ACK
    expectedMsgId = POLL_ACK;
    DW1000Ng::forceTRxOff();
    transmitPoll();
    noteActivity();
}

void handleSent() {
    // status change on sent success
    sentAck = true;
}

void handleReceived() {
    // status change on received success
    receivedAck = true;
}

void transmitPoll() {
    data[0] = POLL;
    DW1000Ng::setTransmitData(data, LEN_DATA);
    DW1000Ng::startTransmit();
}

void transmitRange() {
    data[0] = RANGE;

    /* Calculation of future time */
    byte futureTimeBytes[LENGTH_TIMESTAMP];

	timeRangeSent = DW1000Ng::getSystemTimestamp();
	timeRangeSent += DW1000NgTime::microsecondsToUWBTime(replyDelayTimeUS);
    DW1000NgUtils::writeValueToBytes(futureTimeBytes, timeRangeSent, LENGTH_TIMESTAMP);
    DW1000Ng::setDelayedTRX(futureTimeBytes);
    timeRangeSent += DW1000Ng::getTxAntennaDelay();

    DW1000NgUtils::writeValueToBytes(data + 1, timePollSent, LENGTH_TIMESTAMP);
    DW1000NgUtils::writeValueToBytes(data + 6, timePollAckReceived, LENGTH_TIMESTAMP);
    DW1000NgUtils::writeValueToBytes(data + 11, timeRangeSent, LENGTH_TIMESTAMP);
    DW1000Ng::setTransmitData(data, LEN_DATA);
    DW1000Ng::startTransmit(TransmitMode::DELAYED);
    //Serial.print("Expect RANGE to be sent @ "); Serial.println(timeRangeSent.getAsFloat());
}
int i=0;
void loop() {
    // delay(100);
    // Serial.print("In loop");Serial.println(i);
    // i++;
    // Serial.println("sentAck-> " + sentAck );
    // Serial.println("receivedAck-> " + receivedAck);
    if (!sentAck && !receivedAck) {
        // Serial.println("firstIF");
        // check if inactive
        if (millis() - lastActivity > resetPeriod) {
            resetInactive();
        }
        return;
    }
    // continue on any success confirmation
    if (sentAck) {
        // Serial.println("secondIF");
        sentAck = false;
        DW1000Ng::startReceive();
    }
    if (receivedAck) {
        // Serial.println("thirdIF");
        receivedAck = false;
        // get message and parse
        DW1000Ng::getReceivedData(data, LEN_DATA);
        byte msgId = data[0];
        // Serial.println("msgID-> " + msgId);
        if (msgId != expectedMsgId) {
            // unexpected message, start over again
            //Serial.print("Received wrong message # "); Serial.println(msgId);
            expectedMsgId = POLL_ACK;
            transmitPoll();
            return;
        }
        if (msgId == POLL_ACK) {
            // Serial.println("POLL_ACK");
            timePollSent = DW1000Ng::getTransmitTimestamp();
            timePollAckReceived = DW1000Ng::getReceiveTimestamp();
            expectedMsgId = RANGE_REPORT;
            transmitRange();
            noteActivity();
        } else if (msgId == RANGE_REPORT) {
            expectedMsgId = POLL_ACK;
            float curRange;
            memcpy(&curRange, data + 1, 4);
            Serial.print("range:");Serial.println(curRange/2.54);
            transmitPoll();
            noteActivity();
        } else if (msgId == RANGE_FAILED) {
            Serial.println("Fail to get range");
            expectedMsgId = POLL_ACK;
            transmitPoll();
            noteActivity();
        }
    }
    // delay(1000);
}

// void setup() {
//     Serial.begin(9600);
//     Serial.println("Test message");
// }
// void loop() {int i = 0;Serial.println(i);i++;delay(100);}
