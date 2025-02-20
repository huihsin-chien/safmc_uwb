# 問題

1. built_coord_1, built_coord_2, built_coord_3 的意思?
  1. 這裡只獲取四個 Anchor 之間的距離，沒有要進行計算
2. build_3D_coord 的用途 (為什麼三個 anchor 的座標不是 prior knowledge), bias 是不是在 serial 傳進來之前就被解決了?
  1. self calibration:
    1. 進入瞬間：先算前四個 Anchor（A~D）的座標
    2. 接著將後四個 Anchor（E~H）先當成 Tag，然後把他們的距離記錄下
    3. 當 Anchor 的距離資料量足夠的時候，就會進入 `flying` State
    4. 進入 `flying` 時才會算出 Anchor E~H 的座標，並且離開 Tag 角色、回到 Anchor 角色

    state:
    1. 電腦：用serial給uwb, 有收到就會轉換,target state {1,2,3,s,f} (stand for `built_coord_1`, `built_coord_2`, `built_coord_3`, `self_calibration`, `flying`)
    2. uwb：真正的state，用serial output的方式告訴mediator，存在statemachine裡 {build_coord_1, build_coord_2....., self_calibration, flying}

# 要做的
1. 統一 uwb 送出的資料中的 Anchor/Tag 表示法：都改成 EUI 格式
2. 確認 linux port 可不可以像 windows 現在這樣
3. 確認 match_serial_data 中，build_coord_1,2,3 的必要性。若沒有必要性，統一更改為 build_coord（不能全部改，因為 uwb 角色問題, receiver, sender）
4. 確認uwb data 是不是 precise but bias, 如果有bias,如何處理?
5. 能不能用drone 自身sensorIMU來計算當下的位置(斷掉前+斷掉後移動距離)
6. 長eui以避免同頻道干擾或其他解決方法


