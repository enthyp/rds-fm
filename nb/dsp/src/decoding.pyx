cimport cython
cimport numpy as np
import numpy as np
from collections import deque
from scipy.signal import butter, lfilter
from libc.math cimport cos, pi, sin, ceil, fabs, fmod, sqrt, pow
from functools import reduce


cpdef rrc(double t, double fs, double beta):
    cdef double f, s, num, den

    f = pi / (4 * beta)
    if fabs(t) < 1e-5:
        # return fs * (1 + 1 / f - beta)
        return (1 + 1 / f - beta)
    elif fabs(fabs(t) - 1 / (4 * beta * fs)) < 1e-5:
        s = (1 + 2 / pi) * sin(f) + (1 - 2 / pi) * cos(f)
        # return beta * fs / sqrt(2) * s
        return beta / sqrt(2) * s
    else:
        num = sin(pi * t * fs * (1 - beta))
        num += 4 * beta * t * fs * cos(pi * t * fs * (1 + beta))
        den = pi * t * fs * (1 - pow(4 * beta * t * fs, 2))

        # return fs * num / den
        return num / den


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

    cdef double sps, o_sps
    cdef int filterbank_size
    cdef int filterbank_index
    cdef double soft_index
    cdef int kernel_size
    cdef double[:, :] matched_filter, d_matched_filter
    cdef int buffer_ind
    cdef double[:] filter_buffer
    cdef double Ki, Kp, K0
  
    cdef double B0, A0, A1

    def __init__(
        self, 
        int sampling_rate, 
        double samples_per_symbol, 
        double out_samples_per_symbol,
        int symbol_delay=3,
        int filterbank_size=32,
        double matched_filter_rolloff=0.8,
        double zeta=0.707, 
        double pe_gain=0.5, 
        double nco_gain=1,
        double Bn=0.05
    ):
        self.sps = samples_per_symbol
        self.o_sps = out_samples_per_symbol

        self._prepare_filterbanks(
            filterbank_size, 
            samples_per_symbol,
            symbol_delay, 
            sampling_rate, 
            matched_filter_rolloff
        )
        self._prepare_loop_filter(
            sampling_rate,
            zeta,
            pe_gain,
            nco_gain,
            Bn
        )
        self._prepare_control(
            samples_per_symbol, 
            out_samples_per_symbol,
            symbol_delay
        )

    cdef _prepare_filterbanks(self, int M, double sps, int delay, double fs, double beta):
        cdef int i, j, n_coef, offset
        cdef double hdh_max, t

        hdh_max = 0
        self.filterbank_size = M
        self.kernel_size = <int>(2 * sps * delay) + 1       
        self.matched_filter = np.empty((M, self.kernel_size), dtype=np.double)
        self.d_matched_filter = np.empty((M, self.kernel_size), dtype=np.double)
        offset = <int>(self.kernel_size / 2)

        for i in range(M):
            for j in range(self.kernel_size):
                t = (<double>(j - offset) + <double>i / M) / fs
                self.matched_filter[i][j] = rrc(t, fs, beta)
            
        for i in range(M):
            for j in range(self.kernel_size):
                if i == 0:
                    self.d_matched_filter[i][j] = self.matched_filter[1][j] - self.matched_filter[M - 1][j]                  
                elif i == M - 1:
                    self.d_matched_filter[i][j] = self.matched_filter[0][j] - self.matched_filter[M - 2][j] 
                else:
                    self.d_matched_filter[i][j] = self.matched_filter[i + 1][j] - self.matched_filter[i - 1][j]

                if fabs(self.matched_filter[i][j] * self.d_matched_filter[i][j]) > hdh_max:
                    hdh_max = fabs(self.matched_filter[i][j] * self.d_matched_filter[i][j])

        for i in range(M):
            for j in range(self.kernel_size):
                self.d_matched_filter[i][j] *= 0.06 / hdh_max
          
    cdef _prepare_loop_filter(self, double fs, double zeta, double pe_gain, double nco_gain, double Bn):
        cdef double factor_1, factor_2
        cdef double alpha, beta, a, b        

        Bn *= fs
        factor_1 = 1 / (pe_gain * nco_gain) 
        factor_2 = (Bn / fs) / (zeta + 1 / (4 * zeta))
        self.Kp = factor_1 * 4 * zeta * factor_2
        # self.Kp = 1
        self.Ki = factor_1 * 4 * pow(factor_2, 2)
        # self.Ki = 0.5 * Bn
        self.K0 = nco_gain

        alpha = 1 - Bn
        beta  = 0.22 * Bn
        a = 0.5
        b = 0.495
        
        self.A0 = 1 - a * alpha
        self.A1 = - b * alpha / self.A0
        self.B0 = beta / self.A0
        self.A0 = 1  # not necessary actually

    cdef _prepare_control(self, double sps, double o_sps, int delay):
        self.buffer_ind = 0
        self.filter_buffer = np.zeros((self.kernel_size, ), dtype=np.double)
        self.soft_index = 0
        self.filterbank_index = 0

    cpdef run(self, double[:] samples):
        cdef int i, out_i
        cdef int sample_count
        cdef double tau, tau_delta 
        cdef double pll_phase, time_err
        cdef double integrator_acc, pref_acc
        cdef double filter_out
        cdef np.ndarray [double, ndim=1] output_samples
        cdef np.ndarray[double, ndim=1] output_xs, taus, tau_deltas

        out_i = 0
        sample_count = 0 
        tau = 0
        tau_delta = self.sps / self.o_sps
        integrator_acc = 0
        pref_acc = 0
        output_samples = np.empty(len(samples) * <int>ceil(self.o_sps / self.sps), dtype=np.double) 
        output_xs = np.empty(len(samples) * <int>ceil(self.o_sps / self.sps), dtype=np.double) 
        tau_deltas = np.empty(len(samples) * <int>ceil(self.o_sps / self.sps), dtype=np.double) 
        taus = np.empty(len(samples) * <int>ceil(self.o_sps / self.sps), dtype=np.double) 


        for i in range(len(samples)):
#            print('i: ', i)
            self.push_sample(samples[i])
            
            while out_i < len(samples) * <int>ceil(self.o_sps / self.sps) and self.filterbank_index < self.filterbank_size:
#                print('filterbank_index: ', self.filterbank_index)
#                print('output_index: ', out_i)
                output_samples[out_i] = self.filterbank_output(self.matched_filter, self.filterbank_index)
                output_samples[out_i] /= self.sps
                tau_deltas[out_i] = tau_delta
                taus[out_i] = tau

                output_xs[out_i] = i + self.filterbank_index / self.filterbank_size
                out_i += 1

                if sample_count == self.o_sps:
                    sample_count = 0
           
                    # Estimate time error (maximum likelihood principle).
                    time_err = output_samples[out_i - 1]
                    time_err = time_err * self.filterbank_output(self.d_matched_filter, self.filterbank_index)
                    if time_err > 1:
                        time_err = 1
                    elif time_err < -1:
                        time_err = -1

                    # Super-simple filter.
                    pref_acc = time_err - self.A1 * pref_acc
                    # pref_acc += self.B0 * (self.A0 * time_err - self.A1 * pref_acc)

                    # Lowpass filter - proportional + integrator.
                    integrator_acc += self.Ki * self.B0 * pref_acc # time_err
                    filter_out = integrator_acc + self.Kp * pref_acc # time_err 

                    # Update tau_delta.
                    tau_delta += self.K0 * filter_out
    
                sample_count += 1
                tau += tau_delta
                self.soft_index = tau * self.filterbank_size
                if self.soft_index < 0:
                    self.soft_index = <int>(-self.soft_index / pi) + pi
                    self.soft_index = self.soft_index - fmod(self.soft_index, self.filterbank_size)
                self.filterbank_index = <int>self.soft_index
            
            tau -= 1
            self.soft_index -= self.filterbank_size
            self.filterbank_index = <int>self.soft_index
        
        return output_samples[:out_i], output_xs[:out_i], taus[:out_i], tau_deltas[:out_i] 


    #@cython.initializedcheck(False)
    #@cython.boundscheck(False)
    #@cython.wraparound(False)            
    #@cython.cdivision(True)
    cdef void push_sample(self, double sample):
        self.filter_buffer[self.buffer_ind] = sample
        self.buffer_ind = (self.buffer_ind + 1) % self.kernel_size

    #@cython.initializedcheck(False)
    #@cython.boundscheck(False)
    #@cython.wraparound(False)
    #@cython.cdivision(True)            
    cdef double filterbank_output(self, double[:, :] filterbank, int index) except *:
        cdef int i
        cdef double mult, result

        result = 0
    
        for i in range(1, self.kernel_size + 1):
            mult = self.filter_buffer[(self.buffer_ind - i) % self.kernel_size] 
            mult *= filterbank[index][self.kernel_size - i]
            result += mult

        return result


cdef class BiphaseDecoder:
    """A biphase symbol decoder for RDS physical layer. 

    It decodes data bits that were first differentially encoded and next 
    biphase encoded. 

    It operates on the signal that is obtained as follows: first, amplitude 
    demodulation of 57 kHz subcarrier in an FM demodulated signal from RDS 
    transmitter is performed and next that signal is sampled at appropriate
    time points (cf. SymbolDecoder implementation). Finally, obtained samples
    are normalized to 0/1 binary values.

    In case of errors (when 2 different bits are expected to decode as 0/1 but
    2 identical ones are received instead) a single bit is skipped and normal 
    operation is resumed. This corrects a 1 bit offset error within a symbol, 
    but if 2 identical bits were due to receiver error it introduces a 1 bit 
    offset error anew. It is however corrected upon next pair of identical bits - 
    - no matter if they come from an error or were originally sent. 
  
    """

    cpdef max_run(self, double[::1] samples):
        cdef int i = 1
        cdef int cur_len = 1, max_len = 1
        cdef max_ind = 0

        for i in range(1, len(samples)):
            if samples[i] * samples[i - 1] > 0:
                cur_len += 1
            else:
                max_ind = i
                max_len = max_len if max_len > cur_len else cur_len
                cur_len = 1

        return max_ind, max_len 

    cpdef run(self, double[::1] samples):
        cdef int i, j
        cdef double diff
        cdef np.ndarray[double, ndim=1] output
       
        i = j = 0 
        output = np.empty((samples.shape[0] // 2, ), dtype=np.double)

        while i < len(samples) - 1:
            diff = samples[i] - samples[i + 1]
            if diff > 1e-5:
                output[j] = 1
            elif diff < -1e-5:
                output[j] = 0
            else:
                i += 1
                continue

            j += 1
            i += 2            

        return self._diff_decode(output[:j])        

    cdef _diff_decode(self, double[::1] samples):
        return np.logical_xor(samples[1:], samples[:-1]).astype(np.double)


class BlockGenerator:
    def __init__(self):
        self.parity_matrix = np.array([
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 0],
            [0, 1, 0, 0, 0, 0, 0, 0, 0, 0],
            [0, 0, 1, 0, 0, 0, 0, 0, 0, 0],
            [0, 0, 0, 1, 0, 0, 0, 0, 0, 0],
            [0, 0, 0, 0, 1, 0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0, 1, 0, 0, 0, 0],
            [0, 0, 0, 0, 0, 0, 1, 0, 0, 0],
            [0, 0, 0, 0, 0, 0, 0, 1, 0, 0],
            [0, 0, 0, 0, 0, 0, 0, 0, 1, 0],
            [0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
            [1, 0, 1, 1, 0, 1, 1, 1, 0, 0],
            [0, 1, 0, 1, 1, 0, 1, 1, 1, 0],
            [0, 0, 1, 0, 1, 1, 0, 1, 1, 1],
            [1, 0, 1, 0, 0, 0, 0, 1, 1, 1],
            [1, 1, 1, 0, 0, 1, 1, 1, 1, 1],
            [1, 1, 0, 0, 0, 1, 0, 0, 1, 1],
            [1, 1, 0, 1, 0, 1, 0, 1, 0, 1],
            [1, 1, 0, 1, 1, 1, 0, 1, 1, 0],
            [0, 1, 1, 0, 1, 1, 1, 0, 1, 1],
            [1, 0, 0, 0, 0, 0, 0, 0, 0, 1],
            [1, 1, 1, 1, 0, 1, 1, 1, 0, 0],
            [0, 1, 1, 1, 1, 0, 1, 1, 1, 0],
            [0, 0, 1, 1, 1, 1, 0, 1, 1, 1],
            [1, 0, 1, 0, 1, 0, 0, 1, 1, 1],
            [1, 1, 1, 0, 0, 0, 1, 1, 1, 1],
            [1, 1, 0, 0, 0, 1, 1, 0, 1, 1]
        ], dtype=np.int)

        self.offset_syndrome = {
            0b1111011000: 0,
            0b1111010100: 1,
            0b1001011100: 2,
            0b1111001100: 2,
            0b1001011000: 3
        }
 
        self.corrections = self._prepare_corrections()
    
    def _prepare_corrections(self):
        offset_words = { 
            (0, 0b0011111100),
            (1, 0b0110011000),
            (2, 0b0101101000),
            (2, 0b1101010000),
            (3, 0b0110110100)
        }

        corrections = {}

        for err in (0b1, 0b11):
            for offset_index, offset_word in offset_words:
                for k in range(26):
                    word = offset_word ^ (err << k)
                    err_arr = ((err << k) >> np.arange(25, -1, -1)) % 2
                    word_arr = (word >> np.arange(25, -1, -1)) % 2
                    syndrome = self.syndrome(word_arr)
                    corrections[(syndrome, offset_index)] = err_arr
        return corrections

    def syndrome(self, word):
        product = np.dot(word, self.parity_matrix) % 2
        return reduce(lambda a,b: (a << 1) | b, product, 0)

    def synchronize(self, bits):
        def check_sync(prev_words, word, index):
            for w, ind in prev_words:
                dist_cond = (index - ind) % 26 == 0
                dist = (index - ind) // 26
                word_cond = (word - dist) % 4 == w 
                if dist_cond and word_cond:
                    return True

            return False

        blocks_found = []

        for i in range(len(bits) - 26 + 1):
            s = self.syndrome(bits[i: i + 26])
            offset_word = self.offset_syndrome.get(s, -1)
            
            if offset_word >= 0:
                if check_sync(blocks_found, offset_word, i):
                    return i, offset_word
                else:
                    blocks_found.append((offset_word, i))

        return len(bits), -1

    def process_blocks(self, bits, blocks, initial_offset):
        correction_queue = deque()
        err_count = 0
        expected_offset = initial_offset

        for i in range(0, len(bits) - 26 + 1, 26): 
            s = self.syndrome(bits[i:i + 26])
            offset_word = self.offset_syndrome.get(s, -1)

            if offset_word < 0 or offset_word != expected_offset:
                correction_queue.append(True)
                err_count += 1
                
                try:
                    correction = self.corrections[(s, expected_offset)]
                    data = np.logical_xor(bits[i:i + 26], correction)[:16]
                    data = data.astype(np.int) 
                    got_block = True
                except KeyError:
                    if err_count > 20:
                        return i
                    got_block = False
            else:
                correction_queue.append(False)
                data = bits[i:i + 16]
                got_block = True
            
            expected_offset = (expected_offset + 1) % 4
            if got_block:
                blocks.append((expected_offset, data))

            if len(correction_queue) > 50:
                if correction_queue.popleft():
                    err_count -= 1

        return len(bits) 

    def find_blocks(self, bits):
        # Steps:
        # - iterate over given bits
        #   - calculate syndrome and check if it corresponds to valid offset word
        #   - when 2 valid offset words have been encountered and 
        #     they were spaced fine - assume we're in sync
        #   - once in sync - calculate syndrome for each block and correct errors,
        #     append corrected block to the result list
        # * if too many uncorrectable errors occur (>45 per 50?) - assume sync loss, 
        #   try to acquire again
        i = 0 
        blocks = []        

        while i < len(bits):
            skipped, offset = self.synchronize(bits[i:])
            print('SYNCED: ', i + skipped)
            processed = self.process_blocks(bits[i + skipped:], blocks, offset)
            i += skipped + processed
            print('LOST SYNC: ', i)

        return blocks

