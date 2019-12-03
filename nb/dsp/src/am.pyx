from dsp.fir import WSFilter


class AMDemodulator:
    def __init__(self, fc, fq, M):
        self.lowpass = WSFilter(257, M, fc / fq) 

    def run(self, samples, pilot):
        mixed = samples * pilot
        lp_samples = self.lowpass.real_run(mixed)
        return 2 * lp_samples  

