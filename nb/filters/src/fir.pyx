import numpy as np
from math import cos, sin, pi as PI


cdef class WSFilter:
    def __init__(self, kernel_length, M, fc):
        self.M = M
        self.fc = fc
        self.kernel_length = kernel_length
        self.kernel = [None] * kernel_length
        
        s = 0
        for i in range(kernel_length):
            if i == kernel_length // 2:
                self.kernel[i] = 2 * PI * fc
            else:
                self.kernel[i] = sin(2 * PI * fc * (i - kernel_length / 2)) / (i - kernel_length / 2)

            self.kernel[i] *= (0.42 - 0.5 * cos(2 * PI * i / kernel_length) + 0.08 * cos(4 * PI * i / kernel_length))
            s += self.kernel[i]

        for i in range(kernel_length):
            self.kernel[i] /= s
            
    cpdef run(self, samples):
        lp_len = (samples.shape[0] - self.kernel_length + 1) // self.M + 1
        lp_samples = np.empty(lp_len, dtype=complex)

        for i in range(lp_len):
            offset = i * self.M
            conv_tmp  = [samples[j] * self.kernel[j - offset] for j in range(offset, offset + self.kernel_length)]
            lp_samples[i] = sum(conv_tmp)
            
        return lp_samples
