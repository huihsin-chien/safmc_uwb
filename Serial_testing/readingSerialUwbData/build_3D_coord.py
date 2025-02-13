import numpy as np

clean_anchorABCD_distance_data = {
    ("A", "B"): 3,
    ("A", "C"): 3,
    ("A", "D"): 3,
    ("B", "C"): 3,
    ("B", "D"): 4.242640687119286,
    ("C", "D"): 4.242640687119286
}

def build_distance_matrix(anchorABCD_distance):
    n = 4
    D = np.zeros((n, n))
    for i in range(n):
        for j in range(n):
            if i == j:
                D[i, j ] = 0
            elif i < j:
                D[i,j] = anchorABCD_distance[(chr(i+65), chr(j+65))]
            else:
                D[i,j] = D[j,i]
    return D

def ClassicalMDS(D, dim):   ## can this handle data with noise?
    n = D.shape[0]
    C = np.eye(n) - np.ones((n,n))/n
    B = -C.dot(D**2).dot(C)/2
    eigvals, eigvecs = np.linalg.eig(B)
    idx = np.argsort(eigvals)[::-1] # sort eigvals in descending order
    V = eigvecs[:, idx[:dim]] # select the top dim eigvecs
    L = np.diag(eigvals[idx[:dim]]) # select the top dim eigvals
    X_sub = np.dot(V, np.sqrt(L))
    return X_sub
    # refer to https://tomohiroliu22.medium.com/%E6%A9%9F%E5%99%A8%E5%AD%B8%E7%BF%92-%E5%AD%B8%E7%BF%92%E7%AD%86%E8%A8%98%E7%B3%BB%E5%88%97-68-%E5%A4%9A%E7%B6%AD%E6%A8%99%E5%BA%A6-multidimensional-scaling-caeb1e8c04a3
    # https://en.wikipedia.org/wiki/Multidimensional_scaling 
    
def align_coordinates(X): # Rodrigues' rotation formula
    X_aligned = X - X[0]


    return X_aligned



def build_3D_coord(anchorABCD_distance = clean_anchorABCD_distance_data, dim = 3):
    """
    1. 使用乾淨的 UWB 距離數據，建構距離矩陣
    2. MDS 降維得到初步局部座標
    3.  align_coordinates 進行座標對齊
    """
    D = build_distance_matrix(anchorABCD_distance)
    X = ClassicalMDS(D, dim)
    # print("Initial 3D coordinates:", X)
    X = align_coordinates(X)
    print(X)

if __name__ == "__main__":
    build_3D_coord()






def align_coordinates_3d(X):
    X_aligned = X - X[0]
    angle_z = np.arctan2(X_aligned[1, 1], X_aligned[1, 0])
    Rz = np.array([
        [np.cos(-angle_z), -np.sin(-angle_z), 0],
        [np.sin(-angle_z), np.cos(-angle_z), 0],
        [0, 0, 1]
    ])
    X_aligned = X_aligned.dot(Rz)
    angle_x = np.arctan2(X_aligned[2, 2], X_aligned[2, 1])
    Rx = np.array([
        [1, 0, 0],
        [0, np.cos(-angle_x), -np.sin(-angle_x)],
        [0, np.sin(-angle_x), np.cos(-angle_x)]
    ])
    X_aligned = X_aligned.dot(Rx)
    return X_aligned
