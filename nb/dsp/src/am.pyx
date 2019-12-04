cimport numpy as np
import numpy as np
from dsp.agc import AGC
from dsp.fir import WSFilter


class AMDemodulator:    
    def __init__(self, fc, fq, M, agc=True, agc_low=-20, agc_high=-3, agc_block=256):
        self.apply_agc = agc
        self.agc = AGC(agc_block, agc_low, agc_high)
        self.lowpass = WSFilter(257, M, fc / fq) 

    def run(self, np.ndarray[double, ndim=1] samples, np.ndarray[double, ndim=1] pilot):
        if self.apply_agc:
            samples = self.agc.run(samples)

        mixed = samples * pilot
        lp_samples = 2 * self.lowpass.real_run(mixed)
        return 2 * lp_samples  


