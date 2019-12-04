cimport cython
cimport numpy as np
from libc.math cimport exp, log10, log, pow

import numpy as np


cdef class AGC:
    cdef double alpha

    def __init__(self, double alpha=0.01):
        self.alpha = alpha

    @cython.initializedcheck(False)
    @cython.boundscheck(False)
    @cython.wraparound(False)            
    cpdef run(self, double[:] samples):
        cdef int i
        cdef double mult
        cdef double gain
        cdef double pow_est
        cdef np.ndarray[double, ndim=1] out_samples

        gain = 1 
        pow_est = 0
        out_samples = np.empty((len(samples), ), dtype=np.double)
    
        for i in range(len(samples)):
            mult = samples[i] * gain
            pow_est = self.alpha * mult + (1 - self.alpha) * pow_est

            if pow_est > 1e-6:
                gain *= exp(-0.5 * self.alpha * log(pow_est))
            if gain > 1e6:
                gain = 1e6            
            
            out_samples[i] = mult
 
        return out_samples

