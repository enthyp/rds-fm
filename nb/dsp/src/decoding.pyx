cimport cython
cimport numpy as np
import numpy as np
from scipy.signal import butter, lfilter
from libc.math cimport cos, pi, sin, fmod


cdef class BiphaseDecoder:
    """A biphase symbol decoder for RDS physical layer. 

    It decodes data bits that were first differentially encoded and next 
    biphase encoded and finally pulse-shaped with root-raised-cosine filter. 

    It operates on the signal that is obtained by amplitude demodulation 
    of 57 kHz subcarrier in an FM demodulated signal from RDS transmitter.
   
    """
    
    cdef int clock_rate
    
    def __init__(self, fq=19000, f_clock=1187.5):
        self.clock_step = <int>(fq / f_clock)

    @cython.initializedcheck(False)
    @cython.boundscheck(False)
    @cython.wraparound(False)          
    cpdef run(self, double[::1] samples, double[::1] clock_samples):
        cdef int i, j, out_len
        cdef double integr_acc
        cdef np.ndarray [double, ndim=1] clock, output_samples

        out_len = len(samples) // self.clock_step
        output_samples = np.empty(out_len, dtype=np.double) 
        clock = np.sign(clock_samples)
        j = 0

        while i < out_len:
            j = 0
            integr_acc = 0

            # Multiply by clock and integrate over clock_period.
            while j < self.clock_step:
                
                # Estimate phase error.
                phase_err = 2 * samples[i] * pll_out  # requires lowpass filtering
                
                # Lowpass filter error estimate.
                self.f_state[2] = self.f_state[1]
                self.f_state[1] = self.f_state[0]
                filter_out = phase_err - self.a[0] * self.f_state[1] - self.a[1] * self.f_state[2]

                self.f_state[0] = filter_out
                filter_out = filter_out * self.b[0] + self.f_state[1] * self.b[1] + self.f_state[2] * self.b[2]

                # Update phase.
                pll_phase = fmod(filter_out, 2 * pi)
                pll_out  = cos(pll_phase)
                output_samples[i] = cos(pll_phase - pi / 2)

        return output_samples

    cdef _prepare_clock(self, double[::1] clock_samples):
        cdef int i, j, decim_count
        cdef double clock_val
        cdef np.ndarray [double, ndim=1] clock

        clock = np.sign(clock_samples).astype(np.double)
        i = 0

        # TODO: FUCK. How do we know on which 19 kHz tick 1187.5 Hz tick start???
        while i < len(clock):
            j = i 
            clock_val = clock[j]
            decim_count = 0
            while decim_count < self.clock_step:
                
        
        return clock

    @staticmethod
    def _diff_decode(samples):
        return np.logical_xor(samples[1:], samples[:-1]).astype(np.double)


