import numpy as np

def build_3D_coord(distance_matrix: np.ndarray) -> np.ndarray: 
    d = distance_matrix
    d_ab = d[0][1]
    d_ac = d[0][2]
    d_ad = d[0][3]
    d_bc = d[1][2]
    d_bd = d[1][3]
    d_cd = d[2][3]

    A = [0, 0, 0]

    B_x = d_ab 
    B = [B_x, 0, 0]

    """
                C_x ^ 2 + C_y ^ 2 = d_ac ^ 2
        (C_x - B_x) ^ 2 + C_y ^ 2 = d_bc ^ 2
    => -2 C_x B_x + B_x ^ 2       = d_bc ^ 2 - d_ac ^ 2
    => C_x = (d_bc ^ 2 - d_ac ^ 2 - B_x ^ 2) / (-2 B_x)
       and C_y = (d_ac ^ 2 - C_x ^ 2) ^ 0.5
    """
    C_x = (d_bc ** 2 - d_ac ** 2 - B_x ** 2) / (-2 * B_x)
    C_y = (d_ac ** 2 - C_x ** 2) ** 0.5

    C = [C_x, C_y, 0]

    """
                    D_x ^ 2 + D_y ^ 2 + D_z ^ 2 = d_ad ^ 2 -- [1]
            (D_x - B_x) ^ 2 + D_y ^ 2 + D_z ^ 2 = d_bd ^ 2 -- [2]
    (D_x - C_x) ^ 2 + (D_y - C_y) ^ 2 + D_z ^ 2 = d_cd ^ 2 -- [3]

    From [2] - [1]
    => -2 D_x B_x + B_x ^ 2 = d_bd ^ 2 - d_ad ^ 2
    => D_x = (d_bd ^ 2 - d_ad ^ 2 - B_x ^ 2) / (-2 B_x)

    From [3] - [2]
    => [(D_x - C_x) ^ 2 - (D_x - B_x) ^ 2] + -2 D_y C_y + C_y ^ 2 = d_cd ^ 2 - d_bd ^ 2
    => D_y = (d_cd ^ 2 - d_bd ^ 2 - [(D_x - C_x) ^ 2 - (D_x - B_x) ^ 2] - C_y ^ 2) / (-2 C_y)

    So, from [1], D_z = (d_ad ^ 2 - D_x ^ 2 - D_y ^ 2) ^ 0.5
    """

    D_x = (d_bd ** 2 - d_ad ** 2 - B_x ** 2) / (-2 * B_x)
    D_y = (d_cd ** 2 - d_bd ** 2 - ((D_x - C_x) ** 2 - (D_x - B_x) ** 2) - C_y ** 2) / (-2 * C_y)
    D_z = (d_ad ** 2 - D_x ** 2 - D_y ** 2) ** 0.5

    D = [D_x, D_y, D_z]

    return np.array([np.array(A), np.array(B), np.array(C), np.array(D)])