cimport cython
cimport numpy as np
import numpy as np
from scipy.signal import butter, lfilter
from libc.math cimport cos, pi, sin, fmod

# TODO: IDEA - multiply by sin and cos of (w * t) with w = 19000 kHz
# Then you get IQ samples after low-pass filtering. You can get phase difference for free.


cdef class PLL:
    # Phase-locked loop.
    # source: https://liquidsdr.org/blog/pll-howto/
    # TODO: make it work...
    cdef double [::1] b, a, f_state

    def __init__(self, double fn=0.01, double zeta=0.707, double K=1000):
        cdef t1, t2

        t1 = K / (fn * fn)
        t2 = 2 * zeta / fn
        self.b = np.array([
            (4 * K / t1) * (1 + t2 / 2),
            (8 * K / t1),
            (4 * K / t1) * (1 - t2 / 2)
        ], dtype=np.double)
        self.a = np.array([-2, 1], dtype=np.double)
        self.f_state = np.zeros(3, dtype=np.double)

    @cython.initializedcheck(False)
    @cython.boundscheck(False)
    @cython.wraparound(False)            
    cpdef run(self, double[::1] samples):
        cdef int i
        cdef double pll_phase, pll_out
        cdef double phase_err
        cdef double filter_out
        cdef np.ndarray [double, ndim=1] output_samples

        pll_phase = 0.0
        pll_out = cos(pll_phase)
        output_samples = np.empty(len(samples), dtype=np.double) 

        for i in range(len(samples)):
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


cdef class PLL_2:
    # Another phase-locked loop.
    # source: https://wirelesspi.com/phase-locked-loop-pll-in-a-software-defined-radio-sdr/
    # BTW: it's working!
    cdef double K0, Kd, Kp, Ki, fq, fs

    def __init__(self, double fq, double fs, double zeta=0.707, double pe_gain=0.5, double nco_gain=1, double Bn=0.05):
        self.fq = fq
        self.fs = fs
        self.K0 = nco_gain
        self.Kd = pe_gain  # actually depends on the type of phase error detector used

        Bn *= fs
        factor_1 = 1 / (self.Kd * self.K0) 
        factor_2 = (Bn / fs) / (zeta + 1 / (4 * zeta))
        self.Kp = factor_1 * 4 * zeta * factor_2
        self.Ki = factor_1 * 4 * factor_2 ** 2

    cpdef run(self, double[::1] samples):
        cdef int i
        cdef double pll_phase, phase_err
        cdef double integrator_acc
        cdef double filter_out
        cdef double nco_acc
        cdef np.ndarray [double, ndim=1] output_phases

        pll_phase = 0
        integrator_acc = 0
        nco_acc = 0
        output_phases = np.empty(len(samples), dtype=np.double) 

        for i in range(len(samples)):
            # Estimate phase error.
            phase_err = -sin(2 * pi * self.fq / self.fs * i + pll_phase) * samples[i]

            # Lowpass filter - proportional + integrator.
            integrator_acc += self.Ki * phase_err
            filter_out = integrator_acc + self.Kp * phase_err

            # VCO - update PLL phase value.
            pll_phase += self.K0 * filter_out
                       
            # Save PLL output.
            output_phases[i] = 2 * pi * self.fq / self.fs * i + pll_phase

        return output_phases            


cdef class PLL_3:
    # Yet another pll.
    # source: https://github.com/mwickert/scikit-dsp-comm/blob/master/sk_dsp_comm/rtlsdr_helper.py
    # TODO: figure out what's going on with them filters.
    cdef double [::1] b_lp, a_lp, fstate
    cdef double fs, fq, a, vco_gain, Kt, Kv

    def __init__(self, double fq, double fs, int N=5, double Bn=10, double zeta=0.707):
        self.b_lp, self.a_lp = butter(N, 2 * (fq / 2) / fs)
        self.fstate = np.zeros(N)
        self.Kv = 1 * 2 * pi  # 1Hz/V VCO gain
        self.fs = fs
        self.fq = fq
        fn = 1 / (2 * pi) * 2 * Bn / (zeta + 1/(4 * zeta))  # given Bn in Hz
        self.Kt = 4 * pi * zeta * fn  # loop natural frequency in rad/s
        self.a = pi * fn / zeta

    cpdef run(self, double[::1] samples):
        cdef int i
        cdef double Kv
        cdef double pll_phase, pll_out
        cdef double gain_out
        cdef double filt_in, filt_out, filt_in_last, filt_out_last
        cdef double vco_in, vco_out, vco_in_last, vco_out_last
        cdef double [::1] phase_err
        cdef np.ndarray [double, ndim=1] output_samples, phases

        filt_in_last = filt_out_last = 0.0
        vco_in_last = vco_out_last = 0.0
        pll_phase = 0.0
        pll_out = cos(pll_phase)
        phase_err = np.empty(1)
        output_samples = np.empty(len(samples), dtype=np.double) 
        phases = np.empty(len(samples), dtype=np.double) 

        for i in range(len(samples)):
            # Estimate phase error.
            phase_err[0] = 2 * samples[i] * pll_out
            
            # LPF to remove double frequency term
            phase_err, self.fstate = lfilter(self.b_lp, self.a_lp, phase_err, zi=self.fstate)
            
            # Loop gain
            gain_out = self.Kt / self.Kv * phase_err[0]  # apply VCO gain at VCO

            # Loop filter
            filt_in = self.a * gain_out
            filt_out = filt_out_last + 1 / (2 * self.fs) * (filt_in + filt_in_last)
            filt_in_last = filt_in
            filt_out_last = filt_out
            filt_out = filt_out + gain_out

            # VCO
            vco_in = filt_out + self.fq / (self.Kv / (2 * pi))  # bias to quiescent freq.
            vco_out = vco_out_last + 1 / (2 * self.fs) * (vco_in + vco_in_last)
            vco_in_last = vco_in
            vco_out_last = vco_out
            vco_out = self.Kv * vco_out  # apply Kv
            
            pll_phase = fmod(vco_out, 2 * pi)  # The vco phase mod 2pi
            pll_out = cos(pll_phase)
            output_samples[i] = pll_out
            phases[i] = pll_phase

        return output_samples, phases
 
