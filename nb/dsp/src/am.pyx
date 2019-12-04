cimport numpy as np
import numpy as np
from dsp.agc import AGC
from dsp.fir import WSFilter


class AMDemodulator:    
    def __init__(self, fm, fs, M, agc=True):
        self.apply_agc = agc
        self.agc = AGC()
        self.lowpass = WSFilter(257, M, fm / fs)

    def run(self, np.ndarray[double, ndim=1] samples, np.ndarray[double, ndim=1] pilot):
        if self.apply_agc:
            samples = self.agc.run(pilot)

        mixed = samples * pilot
        lp_samples = self.lowpass.real_run(mixed)
        return 2 * lp_samples  

