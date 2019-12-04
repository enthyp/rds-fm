cimport cython
cimport numpy as np
from libc.math cimport log10, pow

import numpy as np


cdef class AGC:
    cdef int agc_block
    cdef double agc_low, agc_high

    def __init__(self, int agc_block, double agc_low, double agc_high):
        self.agc_block = agc_block
        self.agc_low = agc_low
        self.agc_high = agc_high

    @cython.initializedcheck(False)
    @cython.boundscheck(False)
    @cython.wraparound(False)            
    cpdef run(self, double[:] samples):
        cdef int i, j, l
        cdef double mult
        cdef double alpha
        cdef double pow_rec, pow_prev, pow_acc, db_pow        
        cdef double agc_factor
        cdef np.ndarray[double, ndim=1] out_samples

        pow_acc = 0
        l = len(samples)
        out_samples = np.empty((l, ), dtype=np.double)

        i = 0
        while i < l:
            j = 0
            pow_rec = 0

            # Estimate signal power in a block.
            while j < self.agc_block and i + j < l:
                mult = samples[i + j] * samples[i + j]
                pow_rec += mult
                j += 1
           
            if pow_rec > pow_prev:
                alpha = 0.8187
            else:  
                alpha = 0.99 

            pow_prev = pow_rec
            pow_acc += alpha * pow_acc + (1 - alpha) * pow_rec

            # Modify the gain.
            db_pow = 10 * log10(pow_acc)
            if db_pow < self.agc_low:
                db_pow += 20
                agc_factor = pow(10, 0.1 * db_pow)
            elif db_pow > self.agc_high:
                db_pow = (-4 / 3) * db_pow - 4 
                agc_factor = pow(10, 0.1 * db_pow)
            else:
               agc_factor = 1 

            for j in range(i, i + self.agc_block):
                out_samples[i] = samples[i] * agc_factor
        
            i += self.agc_block

        return out_samples

