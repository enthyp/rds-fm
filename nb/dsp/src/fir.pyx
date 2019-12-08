cimport cython
cimport numpy as np
import numpy as np
from libc.math cimport cos, pi, sin


# TODO: other FIR filters?
 

cdef class FIRFilter:
    """Generic base class for finite impulse response filters."""
    
    cdef int M, kernel_length
    cdef double fc
    cdef double [::1] kernel

    def __init__(self, int kernel_length, int M, double fc, int lowpass=1):
        self.M = M
        self.fc = fc
        self.kernel_length = kernel_length
        self.kernel = np.zeros((kernel_length,), dtype=np.double)

        self._prepare_kernel(lowpass)        

    cdef _prepare_kernel(self, bint lowpass):
        raise NotImplementedError

    @cython.initializedcheck(False)
    @cython.boundscheck(False)
    @cython.wraparound(False)            
    cpdef real_run(self, double[::1] samples):
        cdef int i, j, offset, lp_len
        cdef double acc
        cdef np.ndarray [double, ndim=1] lp_samples 
        
        lp_len = (samples.shape[0] - self.kernel_length + 1) // self.M  
        lp_samples = np.ones(lp_len, dtype=np.double)
        
        for i in range(lp_len):
            acc = 0
            offset = i * self.M
            for j in range(self.kernel_length):
                acc += samples[offset + j] * self.kernel[j] 
            lp_samples[i] = acc

        return lp_samples

    @cython.initializedcheck(False)
    @cython.boundscheck(False)
    @cython.wraparound(False)            
    cdef _complex_run(self, double[::1] samples):
        cdef int i, j, offset, lp_len
        cdef double acc_r, acc_i
        cdef np.ndarray [double, ndim=1] lp_samples 
        
        lp_len = ((samples.shape[0] - 2 * self.kernel_length + 2) // (2 * self.M)) * 2 
        lp_samples = np.ones(lp_len, dtype=np.double)
        
        for i in range(0, lp_len, 2):
            acc_r = acc_i = 0
            offset = i * self.M
            for j in range(self.kernel_length):
                acc_r += samples[offset + 2 * j] * self.kernel[j] 
                acc_i += samples[offset + 2 * j + 1] * self.kernel[j] 
            lp_samples[i] = acc_r
            lp_samples[i + 1] = acc_i

        return lp_samples

    cpdef complex_run(self, np.ndarray [complex, ndim=1] samples):
        cdef np.ndarray [double, ndim=1, mode='c'] interleaved
        interleaved = np.column_stack([samples.real, samples.imag]).flatten()
        f_interleaved = self._complex_run(interleaved).reshape((-1, 2))
        
        return f_interleaved[:, 0] + 1j * f_interleaved[:, 1]


cdef class WSFilter(FIRFilter):
    """A windowed-sinc FIR filter."""

    cdef _prepare_kernel(self, bint lowpass):
        cdef int i
        cdef double s 

        s = 0
        for i in range(self.kernel_length):
            if i == self.kernel_length // 2:
                self.kernel[i] = 2 * pi * self.fc
            else:
                self.kernel[i] = sin(2 * pi * self.fc * (i - self.kernel_length / 2)) / (i - self.kernel_length / 2)

            self.kernel[i] *= (0.42 - 0.5 * cos(2 * pi * i / self.kernel_length) + 0.08 * cos(4 * pi * i / self.kernel_length))
            s += self.kernel[i]

        for i in range(self.kernel_length):
            self.kernel[i] /= s

        if not lowpass:
            for i in range(self.kernel_length):
                self.kernel[i] *= -1;
            if self.kernel_length & 1: 
                self.kernel[self.kernel_length // 2] = 1 + self.kernel[self.kernel_length // 2]

