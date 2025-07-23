import numpy as np
from scipy.optimize import minimize


def ClassicalMDS(D, dim):   ## can this handle data with noise?
    n = D.shape[0]
    C = np.eye(n) - np.ones((n,n))/n
    B = -C.dot(D**2).dot(C)/2
    eigvals, eigvecs = np.linalg.eig(B)
    idx = np.argsort(eigvals)[::-1] # sort eigvals in descending order
    V = eigvecs[:, idx[:dim]] # select the top dim eigvecs
    L = np.diag(eigvals[idx[:dim]]) # select the top dim eigvals
    X_sub = np.dot(V, np.sqrt(L))
    print (" not aligned:")
    print (X_sub)
    return X_sub

    # refer to https://tomohiroliu22.medium.com/%E6%A9%9F%E5%99%A8%E5%AD%B8%E7%BF%92-%E5%AD%B8%E7%BF%92%E7%AD%86%E8%A8%98%E7%B3%BB%E5%88%97-68-%E5%A4%9A%E7%B6%AD%E6%A8%99%E5%BA%A6-multidimensional-scaling-caeb1e8c04a3
    # https://en.wikipedia.org/wiki/Multidimensional_scaling 
    

def align_coordinates(X: np.ndarray) -> np.ndarray: # Rodrigues' rotation formula
    '''
    此程式將 X[0] 當作原點，X[1] 當作 x 軸，X[2]當作 z 軸，進行座標對齊
    1. 以 X[0] (anchor0）為原點，平移所有座標向量
    2. 以 X[0]X[1] 為基準向量（x,0,0），旋轉所有座標向量（anchor 0->1 做為 X 軸正向）
    3. 以 X[0]X[2] 為基準向量（0,y,0），旋轉所有座標向量（anchor 0->2 做為 Y 軸正向）
    4. repeat 2.~3. for 20 times
    5. 因已知 anchor4 有正 z 座標，故如果 X[3][2] 是負數，將所有座標對 XY 平面做鏡像，以確保 Z 軸在上
    6. 因已知 anchor4 有正 z 座標，故如果 X[3][2] 是負數，將所有座標對 XY 平面做鏡像，以確保 Z 軸在上
    '''
    X_aligned = X - X[0]

    # 4. repeat 2.~3. for 20 times
    for i in range(20):
        # 2. 以 X[0]X[1] 為基準向量（x,0,0），旋轉所有座標向量
        u = [1, 0, 0]
        v = X_aligned[1]
        X_1_norm = np.linalg.norm(v)
        u = np.array(u) / np.linalg.norm(u)
        v = np.array(v) / np.linalg.norm(v)
        w = np.cross(u, v)

        w_norm = np.linalg.norm(w)
        if w_norm == 0:
            print("Error: vectors are parallel!")
            exit()
        w = w / w_norm
        cos_theta = np.dot(u, v)  # u · v = |u||v|cos(theta), and both are normalized
        theta = -1 * np.arccos(np.clip(cos_theta, -1.0, 1.0))  # Ensure cos_theta is in valid range
        I = np.identity(3)
        W = np.matrix([[0, -w[2], w[1]],
                    [w[2], 0, -w[0]],
                    [-w[1], w[0], 0]])
        R = I + np.sin(theta) * W + (1 - np.cos(theta)) * np.dot(W, W)
        X_aligned[1] = (np.dot(R, v).A1)*X_1_norm  # Convert to 1D array
        X_aligned[2] = np.dot(R, X_aligned[2]).A1
        X_aligned[3] = np.dot(R, X_aligned[3]).A1
        # plot3D(X_aligned)
        
        # 3. 以X[0]X[2]為基準向量（0,y,0），旋轉所有座標向量
        u = [0, 1, 0]
        v = X_aligned[2]
        X_2_norm = np.linalg.norm(v)
        u = np.array(u) / np.linalg.norm(u)
        v = np.array(v) / np.linalg.norm(v)
        w = np.cross(u, v)

        w_norm = np.linalg.norm(w)
        if w_norm == 0:
            print("Error: vectors are parallel!")
            exit()
        w = w / w_norm
        cos_theta = np.dot(u, v)  # u · v = |u||v|cos(theta), and both are normalized
        theta = -1 * np.arccos(np.clip(cos_theta, -1.0, 1.0))  # Ensure cos_theta is in valid range
        I = np.identity(3)
        W = np.matrix([[0, -w[2], w[1]],
                    [w[2], 0, -w[0]],
                    [-w[1], w[0], 0]])
        R = I + np.sin(theta) * W + (1 - np.cos(theta)) * np.dot(W, W)
        X_aligned[2] = (np.dot(R, v).A1)*X_2_norm  # Convert to 1D array
        X_aligned[1] = np.dot(R, X_aligned[1]).A1
        X_aligned[3] = np.dot(R, X_aligned[3]).A1
        

    # 5. 因已知 anchor 3 有正 y 座標，故如果 X[2][1] 是負數，將所有座標對 XZ 平面做鏡像，以確保 Y 軸在正
    if X_aligned[2][1] < 0:
        for idx in range(4):
            X_aligned[idx][1] = - X_aligned[idx][1]

    # 6. 因已知 anchor4 有正 z 座標，故如果 X[3][2] 是負數，將所有座標對 XY 平面做鏡像，以確保 Z 軸在上
    if X_aligned[3][2] < 0:
        for idx in range(4):
            X_aligned[idx][2] = - X_aligned[idx][2]

    print ( "(", X_aligned[0][0], ",", X_aligned[0][1], ",", X_aligned[0][2], ")\n","(", X_aligned[1][0], ",", X_aligned[1][1], ",", X_aligned[1][2], ")\n", "(", X_aligned[2][0], ",", X_aligned[2][1], ",", X_aligned[2][2], ")\n", "(", X_aligned[3][0], ",", X_aligned[3][1], ",", X_aligned[3][2], ")\n")
    return X_aligned

    #reference: https://openhome.cc/Gossip/WebGL/Rodrigues.html
    # https://www.cnblogs.com/wtyuan/p/12324495.html
    # https://geek-docs.com/numpy/numpy-ask-answer/460_numpy_calculate_rotation_matrix_to_align_two_vectors_in_3d_space.html



def build_3D_coord(distance_matrix: np.ndarray, dim = 3) -> np.ndarray:
    """
    distance_matrix: 一個 n x n 的 np.ndarray，distance_matrix[i][j] 表示 i, j 之間的距離
    return: 一個 4x3 的 np 2d array：即 [coord1, coord2, coord3, coord4]，其中 coord# 是 [x, y, z]

    1. MDS 降維得到初步局部座標
    2. align_coordinates 進行座標對齊
    return 2D array of 3D coordinates

    """
    D = distance_matrix
    X = ClassicalMDS(D, dim)
    X = align_coordinates(X)

    return X

def gps_solve(distances_to_station: list[float], stations_coordinates: list[np.ndarray], initial_guess: np.ndarray=None, tol:float=0.0009) -> np.ndarray or None:
    """多邊定位算法 若有
    :param distances_to_station: list[float], 到各個 anchor 的距離
    :param stations_coordinates: list[np.ndarray], 各 anchor 的座標（每個 anchor 座標都是 len = 3 的 np.ndarray）
    :param initial_guess: np.ndarray=None
    :param tol: float=0.0009
    :return: np.ndarray, 估計的位置, len = 3
    """
    def error(x, c, r):
       
        return sum((np.linalg.norm(x - c[i]) - r[i]) ** 2 for i in range(len(c)))
        
        
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


