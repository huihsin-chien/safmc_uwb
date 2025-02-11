import numpy as np
clean_anchorABCD_distance_data = {
    ("A", "B"): 1,
    ("A", "C"): 2,
    ("A", "D"): 3,
    ("B", "C"): 4,
    ("B", "D"): 5,
    ("C", "D"): 6
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

def mds(D, dim):
    n = D.shape[0]
    C = np.eye(n) - np.ones((n,n))/n
    B = -C.dot(D**2).dot(C)/2
    eigvals, eigvecs = np.linalg.eig(B)
    idx = np.argsort(eigvals)[::-1] # sort eigvals in descending order
    # eigvals = eigvals[idx]
    # eigvecs = eigvecs[:,idx]
    # W = eigvecs[:,:dim].dot(np.diag(np.sqrt(eigvals[:dim])))
    # return W

def align_coordinates(X):
    n = X.shape[0]
    X_mean = X.mean(axis = 0)
    X_centered = X - X_mean
    U, S, V = np.linalg.svd(X_centered)
    R = V.T.dot(U.T)
    return X_centered.dot(R)


def build_3D_coord(anchorABCD_distance = clean_anchorABCD_distance_data, dim = 3):
    """
    1. 使用乾淨的 UWB 距離數據，建構距離矩陣
    2. MDS 降維得到初步局部座標
    3.  align_coordinates 進行座標對齊
    """
    D = build_distance_matrix(anchorABCD_distance)
    X = mds(D, dim)
    X = align_coordinates(X)