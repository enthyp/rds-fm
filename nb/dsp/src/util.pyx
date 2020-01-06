cimport cython
cimport numpy as np
import numpy as np

@cython.initializedcheck(False)
@cython.boundscheck(False)
@cython.wraparound(False)            
cpdef lcs(long[::1] A, long[::1] B):
    cdef int i_A, i_B, r, c, l_A, l_B
    cdef long[:, :] lcs_arr
    cdef long[::1] A_ind, B_ind 

    l_A, l_B = len(A), len(B)
    lcs_arr = np.zeros((len(B) + 1, len(A) + 1), dtype=long)        
    A_ind = np.zeros((len(A), ), dtype=long)        
    B_ind = np.zeros((len(B), ), dtype=long)        

    for r in range(1, l_B + 1):
        for c in range(1, l_A + 1):
            if B[r - 1] == A[c - 1]:
                lcs_arr[r][c] = lcs_arr[r - 1][c - 1] + 1
            else:
                if lcs_arr[r - 1][c] > lcs_arr[r][c - 1]: 
                    lcs_arr[r][c] = lcs_arr[r - 1][c]
                else:
                    lcs_arr[r][c] = lcs_arr[r][c - 1]

    i_A, i_B = l_A - 1, l_B - 1
    r, c = l_B, l_A
    while r >= 1 and c >= 1:
        if lcs_arr[r][c] == lcs_arr[r - 1][c]:
            r -= 1
        elif lcs_arr[r][c] == lcs_arr[r][c - 1]:
            c -= 1
        else:
            A_ind[i_A] = c - 1
            B_ind[i_B] = r - 1
            r -= 1
            c -= 1
            i_A -= 1
            i_B -= 1

    return np.array(lcs_arr), np.array(A_ind[i_A + 1:]), np.array(B_ind[i_B + 1:])

