import numpy as np
import pandas as pd

# ========== 基本輸入 ==========
# anchors (x,y)
anchors = np.array([[0.0, 0.0],
                    [8.0, 0.0],
                    [4.0, 4.0]])

# 測到的距離 (m)
ranges = np.array([45.08, 44.32, 49.57])

# 估計位置 (x, y)
p_hat = np.array([8.23, 44.34])

# 校正表 (Real Length 與 Std)
calib = pd.DataFrame({
    "Real Length": [0,3,5,6,9,10,12,15,18,20,21,24,25,27,30,33,35,36,39,40,42,45,48,50,51,54,55,57,60,63,65],
    "Std": [0.148242,0.043708,0.078889,0.030269,0.068376,0.075397,0.053864,0.198789,0.084648,0.065091,0.123263,
            0.044865,0.05268,0.040793,0.036058,0.284102,0.054037,0.816694,1.557951,0.070185,0.196966,0.040315,
            0.030639,0.661929,0.203289,1.444896,0.191301,0.032451,0.597958,0.049518,0.089583]
})

# ========== 計算每個 anchor 幾何距離 ==========
diff = p_hat - anchors  # N x 2
d_geom = np.linalg.norm(diff, axis=1)  # N

# ========== 插值標準差 ==========
calib_d = calib["Real Length"].values
calib_std = calib["Std"].values

# 線性插值，超出範圍用邊界值
std_interp = np.interp(d_geom, calib_d, calib_std,
                       left=calib_std[0], right=calib_std[-1])

# 設定最小值，避免太小導致數值爆炸
sigma_floor = 0.02
std_interp = np.maximum(std_interp, sigma_floor)

# ========== 幾何矩陣 H ==========
H = (p_hat - anchors) / d_geom[:, None]  # N x 2

# 權重矩陣 W
W = np.diag(1.0 / (std_interp**2))

# 協方差 Σ_p = inv(H^T W H)
HTWH = H.T @ W @ H
Sigma_p = np.linalg.inv(HTWH)

# ========== 結果 ==========
sigma_x = np.sqrt(Sigma_p[0, 0])
sigma_y = np.sqrt(Sigma_p[1, 1])
cov_xy  = Sigma_p[0, 1]

print("位置協方差矩陣 Σ_p =\n", Sigma_p)
print(f"σ_x = {sigma_x:.4f} m, σ_y = {sigma_y:.4f} m, Cov_xy = {cov_xy:.4f}")

# ========== 存成 CSV ==========
# 1. 每個 anchor 的資訊
res_df = pd.DataFrame({
    "d_geom (m)": d_geom,
    "std_interpolated (m)": std_interp
}, index=["anchor1","anchor2","anchor3"])

res_df.to_csv("anchor_errors.csv", index=True)
print("已存成 anchor_errors.csv")

# 2. 協方差矩陣
cov_df = pd.DataFrame(Sigma_p, index=["x","y"], columns=["x","y"])
cov_df.to_csv("position_covariance.csv")
print("已存成 position_covariance.csv")
