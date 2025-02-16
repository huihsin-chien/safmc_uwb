## main.py

* [X]  record sampling rate in csv files
* [X]  紀錄 anchor 之間距離-> 惠心
* [ ]  紀錄 anchor 之間座標-> 惠心
* [X]  加一個 state 記錄 dCD
* [ ]  tag 位置陣列(全域變數) -> 惠心
* [X]  清洗  `distance_between_anchors_and_anchors`
  * [X]  取四分位數
  * [X]  把零清掉
* [ ]  previous data 陣列紀錄所有anchor 跟 tag之間的距離，若此次0.1秒內沒有距離資料就用此距離補上，並且每兩秒若沒更新就以其他anchor的資料做計算，直到此anchor有回傳距離
* [ ]  implement multilateration
* [X]  handle self-calibration state machine
  * [X]  built_coord_1: 得到距離 dAB + dAC -> 惠心
  * [ ]  built_coord_2: 得到距離 dBC，(之後應該要再多加一個anchor D 用來建立 3D 坐標系)，計算出 3D 座標-> 惠心
  * [ ]  self_calibration: 和ＥＦＧＨanchor TWR，得到距離，求出座標-> 惠心
  * [ ]  flying: 與 tag TWR
* [X]  如何清理得到的 data? pooling 距離時要加 filter？避免 noise & 離群值
* [ ]  實際驗證程式功能（得到anchor 座標->tag multilateration）
* [ ]  讓 sample rate record in csv 更易讀

## Anchors & Tags

* [X]  修改 blink rate 讓資料多一點
* [ ]  create system with different/best PRF, data Rate, preamble length, channel -> 亞彤
* [ ]  add more tags and anchors 進入系統
* [X]  在 self_calibration 時，增加更多 anchor 來求得 3D 座標系（at least 4 anchorABCD for 建立3D座標，1 additinal anchor, 2 tag)
* [ ]  統一 tag + anchor sample rate 表示方式
