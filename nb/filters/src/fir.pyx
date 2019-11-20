cimport cython
cimport numpy as np
import numpy as np
from libc.math cimport cos, pi, sin


cdef class WSFilter:
    cdef int M, kernel_length
    cdef double fc
    cdef double [::1] kernel

    def __init__(self, int kernel_length, int M, double fc):
        self.M = M
        self.fc = fc
        self.kernel_length = kernel_length
        self.kernel = np.zeros((kernel_length,), dtype=np.double)
        
        cdef int i
        cdef double s 

        s = 0
        for i in range(kernel_length):
            if i == kernel_length // 2:
                self.kernel[i] = 2 * pi * fc
            else:
                self.kernel[i] = sin(2 * pi * fc * (i - kernel_length / 2)) / (i - kernel_length / 2)

            self.kernel[i] *= (0.42 - 0.5 * cos(2 * pi * i / kernel_length) + 0.08 * cos(4 * pi * i / kernel_length))
            s += self.kernel[i]

        for i in range(kernel_length):
            self.kernel[i] /= s

    @cython.initializedcheck(False)
    @cython.boundscheck(False)
    @cython.wraparound(False)            
    cdef _complex_run(self, double[::1] samples):
        cdef int i, j, offset, lp_len
        cdef double acc_r, acc_i
        cdef np.ndarray [double, ndim=1] lp_samples 
        
        lp_len = ((samples.shape[0] - 2 * self.kernel_length + 2) // (2 * self.M)) * 2 
        lp_samples = np.ones(lp_len, dtype=np.double)
        
        i = 0
        while i < lp_len:
            j = 0
            acc_r = acc_i = 0
            offset = i * self.M
            while j < 2 * self.kernel_length:
                acc_r += samples[offset + j] * self.kernel[j] 
                acc_i += samples[offset + j + 1] * self.kernel[j] 
                j += 2
            lp_samples[i] = acc_r
            lp_samples[i + 1] = acc_i
            i += 2            

        return lp_samples

    cpdef complex_run(self, np.ndarray [complex, ndim=1] samples):
        cdef np.ndarray [double, ndim=1] interleaved
        interleaved = np.column_stack([samples.real, samples.imag]).flatten()
       
        return self._complex_run(interleaved)

