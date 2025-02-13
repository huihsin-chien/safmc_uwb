## main.py

* [X]  record sampling rate in csv files
* [ ]  implement multilateration
* [ ]  multilateration: 有蟲
* [ ]  handle self-calibration state machine
  * [ ]  built_coord_1: 得到距離 dAB + dAC
  * [ ]  built_coord_2: 得到距離 dBC，(之後應該要再多加一個amchor D 用來建立 3D 坐標系)，計算出 3D 座標
  * [ ]  self_calibration: 和ＥＦＧＨanchor TWR，得到距離，求出座標
  * [ ]  flying: 與 tag TWR
* [ ]  讓 sample rate record in csv 更易讀
* [ ]  實際驗證程式功能（得到anchor 座標->tag multilateration）
* [ ]  如何清理得到的 data? pooling 距離時要加 filter？避免 noise & 離群值

## Anchors & Tags

* [ ]  create system with different/best PRF, data Rate, preamble length, channel
* [ ]  修改 blink rate 讓資料多一點
* [ ]  add more tags and anchors 進入系統
* [ ]  在 self_calibration 時，增加更多 anchor 來求得 3D 座標系（at least 4 anchorABCD for 建立3D座標，1 additinal anchor, 2 tag)
* [ ]  統一 tag + anchor sample rate 表示方式
