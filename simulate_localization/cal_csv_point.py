import pandas as pd
from collections import deque, defaultdict
from datetime import datetime, timedelta
import numpy as np 
from scipy.optimize import minimize

# 讀取 csv
df = pd.read_csv(r"C:\Users\jianh\OneDrive\Desktop\ToonsRobotics\safmc\safmc_uwb\simulate_localization\0813_3tag_1anchor_test_data\line2_2.csv")
result_csv=r"C:\Users\jianh\OneDrive\Desktop\ToonsRobotics\safmc\safmc_uwb\simulate_localization\trilateration_output_2_2.csv"

# 轉換 Timestamp 為 datetime
df['Timestamp'] = pd.to_datetime(df['Timestamp'])
# 只記錄每個 tag id 最新一筆資料
latest_tag_data = {}

def two_tag_trilateration(tag1_distance, tag2_distance):
    # x_c = (a**2 - c_true**2 + b_true**2) / (2*a)
    #     y_c = np.sqrt(abs(b_true**2 - x_c**2))
    x = (8**2 - tag2_distance**2 + tag1_distance**2) / (2*8)
    y = np.sqrt(abs(tag1_distance**2 - x**2))
    return (x, y)

# def gradient_multilateraion(tag1_distance, tag2_distance, tag3_distancd, initial_guess):


def error_function(x, anchors, distances):
    error = 0.0
    for i in range(len(anchors)):
        dist = np.linalg.norm(x - anchors[i])
        error += (dist - distances[i]) ** 2
    return error

def gps_solve_gradient(anchors, distances, initial_guess=np.zeros(2), tol=1e-4, max_iter=200):
    x = initial_guess
    alpha = 0.01  # learning rate
    for _ in range(max_iter):
        grad = np.zeros(2)
        h = 1e-6
        for d in range(2):
            x1 = np.copy(x)
            x2 = np.copy(x)
            x1[d] += h
            x2[d] -= h
            grad[d] = (error_function(x1, anchors, distances) - error_function(x2, anchors, distances)) / (2 * h)
        x_new = x - alpha * grad
        if np.linalg.norm(x_new - x) < tol:
            break
        x = x_new
    return x
def gps_solve(distances_to_station: list[float], stations_coordinates: list[np.ndarray], initial_guess: np.ndarray=None, tol:float=0.0009) -> np.ndarray or None:
    """多邊定位算法 若有
    :param distances_to_station: list[float], 到各個 anchor 的距離
    :param stations_coordinates: list[np.ndarray], 各 anchor 的座標（每個 anchor 座標都是 len = 3 的 np.ndarray）
    :param initial_guess: np.ndarray=None
    :param tol: float=0.0009
    :return: np.ndarray, 估計的位置, len = 3
    """
    def error(x, c, r):
        # return sum([(np.linalg.norm(x - c[i]) - r[i]) ** 2 for i in range(min(len(c), len(r)))])
        return sum((np.linalg.norm(x - c[i]) - r[i]) ** 2 for i in range(len(c)))
        # return sum((x[0] - c[i][0]) ** 2 + (x[1] - c[i][1]) ** 2 + (x[2] - c[i][2]) ** 2 for i in range(len(c)))

    # 如果沒有初始推測    
    if initial_guess is None:
        # 為初始推測計算權重
        l = len(stations_coordinates)
        S = sum(distances_to_station)
        W = []
        if all(S - w != 0 for w in distances_to_station):
            W = [((l - 1) * S) / (S - w) for w in distances_to_station]
        else:
            print("Error: Only one distance provided")
            return None
            
        # 取得初始推測
        Length = len(W)
        x0 = sum([W[i] * stations_coordinates[i] for i in range(Length)])
    else:
        x0 = initial_guess
    
    # optimize distance from signal origin to border of spheres
    return minimize(
        error, x0, 
        args=(stations_coordinates, distances_to_station), method='Nelder-Mead',
        tol=tol
    ).x 


results = []
tag_positions = np.array([[0, 0], [8, 0], [4, -5]])
# previous_grad_est = np.zeros(2)
previous_nelder_est = np.zeros(2)
sliding_window = 100
tri_slide_result = []
grad_slide_result = []
nelder_slide_result = []


for _, row in df.iterrows():
    tag_id = int(str(row['Tag EUI']), 16)  # 假設 Tag EUI 是 16 進位字串
    now = row['Timestamp']
    distance = row['Distance (m)']
    latest_tag_data[tag_id] = (distance, now)
    # 只有當 257 和 514 和 771 都有資料時才進行 trilateration 計算
    if 257 in latest_tag_data and 514 in latest_tag_data and 771 in latest_tag_data:
        trilateration_result = two_tag_trilateration(latest_tag_data[257][0], latest_tag_data[514][0])
        # print(f"Time: {now}, Trilateration Result: {trilateration_result}")
        results.append({
            'Time': now,
            'tri_X': trilateration_result[0],
            'tri_Y': trilateration_result[1]
        })
        if len(tri_slide_result) >= sliding_window:
            tri_slide_result.pop(0)
        tri_slide_result.append(trilateration_result)   
        tri_slide_est = np.mean(tri_slide_result, axis=0)
        results.append({
            'Time': now,
            'slide_tri_X': tri_slide_est[0],
            'slide_tri_Y': tri_slide_est[1]
        })

        grad_est = gps_solve_gradient(tag_positions, [latest_tag_data[257][0], latest_tag_data[514][0], latest_tag_data[771][0]], 
                                  initial_guess=np.array(trilateration_result))
        # previous_grad_est = grad_est
        # print(f"Gradient Estimate: {grad_est}")
        results.append({
            'Time': now,
            'Gradient_X': grad_est[0],
            'Gradient_Y': grad_est[1]
        })
        if len(grad_slide_result) >= sliding_window:
            grad_slide_result.pop(0)
        grad_slide_result.append(grad_est)
        grad_slide_est = np.mean(grad_slide_result, axis=0)

        results.append({
            'Time': now,
            'slide_grad_X': grad_slide_est[0],
            'slide_grad_Y': grad_slide_est[1]
        })
        nelder_result = gps_solve([latest_tag_data[257][0], latest_tag_data[514][0], latest_tag_data[771][0]], tag_positions, 
                                  initial_guess=trilateration_result)
        # print(f"Nelder-Mead Result: {nelder_result}")
        previous_nelder_est = nelder_result
        if nelder_result[1] < 0:
            continue
        results.append({
            'Time': now,
            'Nelder_X': nelder_result[0],
            'Nelder_Y': nelder_result[1]
        })
        if len(nelder_slide_result) >= sliding_window:
            nelder_slide_result.pop(0)
        nelder_slide_result.append(nelder_result)
        nelder_slide_est = np.mean(nelder_slide_result, axis=0)
        results.append({
            'Time': now,
            'slide_nelder_X': nelder_slide_est[0],
            'slide_nelder_Y': nelder_slide_est[1]
        })

# # 印出每個 tag id 最新的距離與時間
for tag_id, (distance, now) in latest_tag_data.items():
    print(f"Latest for tag {tag_id:04X}: {distance} m at {now}")
    print(tag_id, hex(tag_id))

# 將結果寫入 csv 檔案
if results:
    results_df = pd.DataFrame(results)
    results_df.to_csv(result_csv, index=False)