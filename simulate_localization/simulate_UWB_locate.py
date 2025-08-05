import numpy as np
import matplotlib.pyplot as plt

tag_position = {(0, 0), (8, 0)} # 場地上 tag 的位置，可有 1 ~ 4 個 tag。
# (0, 0), (8, 0) / (0, 0), (8, 0), (4, 0) / (0, 0), (8, 0), (4, 0), (4, -3) / (0, 0), (8, 0), (-3, 0) / (0, 0), (8, 0), (-3, 0), (-3, 8)

def main():
    # 1. 在 63*8 的長方形場地上，產生 X 狀的 anchor 座標點
    # 2. 讓每個 tag 都產生到 anchor 的距離
    # 3. 將這些距離加上 noise (多種 noise with diff noise type / std deviation / mean)
    # 4. 計算含 noise 距離的座標
    # 5. plot real location v.s. location with noise

    anchors_position = []
    anchor2tag_distance = {}
    for i in range(100):
        anchors_position.append((i*0.08, i*0.08*63/8))
        anchors_position.append((i*0.08, 8 - i*0.08*63/8))
        anchors_position.append((0.63*i, 0))
        anchors_position.append((0.63*i, 8))
        anchors_position.append((63, 0.08*i))

    anchors_position  = np.array(anchors_position)
    for tag in tag_position:
        tag = np.array(tag)
        distances = np.linalg.norm(anchors_position - tag, axis=1)
        anchor2tag_distance[tag] = distances
    



if __name__ == "__main__":
    main()