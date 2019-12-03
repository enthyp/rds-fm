cimport numpy as np
from libc.math cimport atan2, pi 

import numpy as np


cdef class FMDemodulator:        
    def __cinit__(self):
        pass

    cdef _run(self, double[::1] samples):
        cdef int i 
        cdef double angle_c, angle_p, angle_diff
        cdef np.ndarray[double, ndim=1] dm_samples

        dm_samples = np.empty(shape=((len(samples) - 2) // 2, ), dtype=np.double)
        
        for i in range(len(dm_samples)):
            angle_c = atan2(samples[2 * i + 2], samples[2 * i + 3])
            angle_p = atan2(samples[2 * i], samples[2 * i + 1])
            angle_diff = angle_c - angle_p
            
            if (angle_diff > pi):
                angle_diff = 2 * pi - angle_diff
            elif (angle_diff < -pi):
                angle_diff = -2 * pi - angle_diff
            dm_samples[i] = angle_diff
        
        return dm_samples         


    cpdef run(self, np.ndarray [complex, ndim=1] samples):
        cdef np.ndarray [double, ndim=1, mode='c'] interleaved
        interleaved = np.column_stack([samples.real, samples.imag]).flatten()
        d_interleaved = self._run(interleaved) 
    
        return d_interleaved


