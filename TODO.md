## main.py

* [ ]  有時候 3D 位置解算會算不出來 該怎麼辦
* [ ] 實作 state 從 self_calibration -> flying 的轉換
* [ ] 新增更多 anchor 做 self_calibration
  * [ ] anchor_locations 需要更多除了 anchor ABCD 以外的資料 at line 341
* [ ] if self_calibration hasn't implemented yet, 惠心 can skip self_calibration part and go testing flying part 
* [X]  發現state 轉換無法從 built_coord_3 變成 self_calibration
* [X]  record sampling rate in csv files
* [X]  紀錄 anchor 之間距離-> 惠心
* [X]  紀錄 anchor 之間座標-> 惠心
* [X]  加一個 state 記錄 dCD
* [X]  tag 位置陣列 -> 惠心
* [X]  清洗  `distance_between_anchors_and_anchors`

  * [X]  取四分位數
  * [X]  把零清掉
* [X]  self.previous_pooled_data = {} 紀錄所有anchor 跟 tag之間的距離，若此次 0.1 秒內沒有距離資料就用此距離補上，並且每兩秒若沒更新就以其他anchor的資料做計算，省略此 anchor 距離送入 gps_solve，直到此 anchor 有回傳距離
* [X]  把state change 改成由main.py傳serial輸出給anchor
* [X]  implement multilateration
* [X]  handle self-calibration state machine

  * [X]  built_coord_1: 得到距離 dAB + dAC -> 惠心
  * [X]  built_coord_2: 得到距離 dBC，(之後應該要再多加一個anchor D 用來建立 3D 坐標系)，計算出 3D 座標-> 惠心
  * [X]  self_calibration: 和ＥＦＧＨanchor TWR，得到距離，求出座標-> 惠心
  * [X]  flying: 與 tag TWR 
* [X]  如何清理得到的 data? pooling 距離時要加 filter？避免 noise & 離群值 -> 亞彤
* [ ]  實際驗證程式功能（得到anchor 座標->tag multilateration）
* [ ]  讓 sample rate record in csv 更易讀

## Anchors & Tags

* [X]  修改 blink rate 讓資料多一點 -> 亞彤
* [X]  create system with different/best PRF, data Rate, preamble length, channel -> 亞彤
* [ ]  add more tags and anchors 進入系統
* [X]  在 self_calibration 時，增加更多 anchor 來求得 3D 座標系（at least 4 anchorABCD for 建立3D座標，1 additinal anchor, 2 tag)
* [ ]  統一 tag + anchor sample rate 表示方式
