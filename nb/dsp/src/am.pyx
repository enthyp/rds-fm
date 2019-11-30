from fir import WSFilter


class AMDemodulator:
    def __init__(self, fc, fq):
        self.lowpass = WSFilter(257, 1, fc / fq)  # Replace windowed-sinc with root-raised-cosine!

    def run(self, samples, pilot):
        mixed = samples * pilot
        lp_samples = self.lowpass.real_run(mixed)
        return 2 * lp_samples  

