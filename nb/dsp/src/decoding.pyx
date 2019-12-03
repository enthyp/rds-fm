cimport cython
cimport numpy as np
import numpy as np
from scipy.signal import butter, lfilter
from libc.math cimport cos, pi, sin, fmod, sqrt, pow


cdef rrc(double t, double fs, double beta):
    f = pi / (4 * beta)
    if t == 0:
        return fs * (1 + 1 / f - beta)
    elif abs(t) == 1 / (4 * beta * fs):
        s = (1 + 2 / pi) * sin(f) + (1 - 2 / pi) * cos(f)
        return beta * fs / sqrt(2) * s
    else:
        num = sin(pi * t * fs * (1 - beta))
        num += 4 * beta * t * fs * cos(pi * t * fs * (1 + beta))
        den = pi * t * fs * (1 - pow(4 * beta * t * fs, 2))

        return fs * num / den


cdef class SymbolDecoder:
    """A synchronizing symbol decoder for symbols that were pulse-shaped 
    with a root-raised-cosine filter.

    Synchronization is performed using polyphase filterbanks and maximum
    likelihood estimation, as described in:
    
    Fredric J. Harris and Michael Rice,
      "Multirate Digital Filters for Symbol Timing Synchronization
      in Software Defined Radios," IEEE Journal on Selected Areas
      of Communications, vol. 19, no. 12, December, 2001, pp.
      2346-2357.

    """ 

    cdef int filterbank_index
    cdef double soft_index
    cdef double[:, :] matched_filter, d_matched_filter
    cdef double[:] filter_buffer
    cdef double Ki, Kp, K0
  
    def __init__(
        self, 
        int sampling_rate, 
        double samples_per_symbol, 
        int symbol_delay=3,
        int filterbank_size=32,
        double matched_filter_rolloff=0.8,
        double zeta=0.707, 
        double pe_gain=0.5, 
        double nco_gain=1,
        double Bn=0.05
    ):
        self._prepare_filterbanks(
            filterbank_size, 
            samples_per_symbol,
            symbol_delay, 
            sampling_rate, 
            matched_filter_rolloff
        )
        self._prepare_loop_filter(
            1 / sampling_rate,
            zeta,
            pe_gain,
            nco_gain,
            Bn
        )
        self._prepare_control(
            samples_per_symbol, 
            symbol_delay
        )

    cdef _prepare_filterbanks(self, int M, double sps, int delay, double fs, double beta):
        cdef int i, j, n_coef, offset

        n_coef = <int>(2 * sps * delay) + 1       
        self.matched_filter = np.empty((M, n_coef), dtype=np.double)
        self.d_matched_filter = np.empty((M, n_coef), dtype=np.double)
        offset = <int>(n_coef / 2)

        for i in range(M):
            for j in range(n_coef):
                t = ((j - offset) + i / M) / fs
                self.matched_filter[i][j] = rrc(t, fs, beta)
         
        for i in range(M):
            for j in range(n_coef):
                if i == 0:
                    self.d_matched_filter[i][j] = self.matched_filter[1][j] - self.matched_filter[M - 1][j]                  
                elif i == M - 1:
                    self.d_matched_filter[i][j] = self.matched_filter[0][j] - self.matched_filter[M - 2][j] 
                else:
                    self.d_matched_filter[i][j] = self.matched_filter[i + 1][j] - self.matched_filter[i - 1][j]

    cdef _prepare_loop_filter(self, double fs, double zeta, double pe_gain, double nco_gain, double Bn):
        cdef double factor_1, factor_2
        self.b = np.empty((3, ), dtype=np.double)
        self.a = np.empty((3, ), dtype=np.double)
        
        Bn *= fs
        factor_1 = 1 / (pe_gain * nco_gain) 
        factor_2 = (Bn / fs) / (zeta + 1 / (4 * zeta))
        self.Kp = factor_1 * 4 * zeta * factor_2
        self.Ki = factor_1 * 4 * pow(factor_2, 2)
        self.K0 = nco_gain

    cdef _prepare_control(self, double sps, int delay):
        self.filter_buffer = np.empty((2 * sps * delay + 1, ), dtype=np.double)
        self.soft_index = 1
        self.filterbank_index = 0

    cpdef run(self, samples):
        cdef int i, out_i
        cdef double pll_phase, time_err
        cdef double integrator_acc
        cdef double filter_out
        cdef double nco_acc
        cdef np.ndarray [double, ndim=1] output_phases

        out_i = 0
        pll_phase = 0
        integrator_acc = 0
        nco_acc = 0
        output_phases = np.empty(len(samples), dtype=np.double) 

        for i in range(len(samples)):
            # Prepare interpolate value.
            if soft_index < 0:
                self.filterbank_index = <int>()
                self._run_filterbank(self.filterbank_index)
                self.soft_index = 1 - self.soft_index
                
            
            
            # TODO: Upsample.

            # Lowpass filter - proportional + integrator.
            integrator_acc += self.Ki * time_err
            filter_out = integrator_acc + self.Kp * time_err 

            # VCO - update PLL phase value.
            self.soft_index -= (self.K0 * filter_out + )


cdef class BiphaseDecoder:
    """A biphase symbol decoder for RDS physical layer. 

    It decodes data bits that were first differentially encoded and next 
    biphase encoded. 

    It operates on the signal that is obtained as follows: first, amplitude 
    demodulation of 57 kHz subcarrier in an FM demodulated signal from RDS 
    transmitter is performed and next that signal is sampled at appropriate
    time points (cf. SymbolDecoder implementation). Finally, obtained samples
    are normalized to 0/1 binary values.
   
    """

    cpdef run(self, double[::1] samples, double[::1] clock_samples):
        cdef double[::1] deltas

        deltas = np.diff(samples)[::2]
        return self._diff_decode(deltas)

    cdef _diff_decode(self, double[::1] samples):
        return np.logical_xor(samples[1:], samples[:-1]).astype(np.double)

