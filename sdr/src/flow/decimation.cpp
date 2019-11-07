#include <iostream>
#include <algorithm>
#include "const.h"
#include "flow/decimation.h"


template <typename T_in, typename T_out>
decimator<T_in, T_out>::decimator(int m_factor, double fc, int kernel_length)
  : m_factor {m_factor},
    fc {fc},
    kernel_length {kernel_length},
    kernel (kernel_length, 0.),
    decimated_buffer {0},
    working {false},
    window_cnt {0}
    {
      double sum = 0;

      // Windowed-sinc filter (Blackman window).
      // TODO: can be generalized (strategy pattern) to any FIR filter!
      for (int i = 0; i < kernel_length; i++) {
        if (i == kernel_length / 2) {
          kernel[i] = 2 * PI * fc;
        } else {
          kernel[i] = sin(2 * PI * fc * (i - kernel_length / 2.)) / (i - kernel_length / 2.);
        }

        kernel[i] *= (0.42 - 0.5 * cos(2 * PI * i / kernel_length) + 0.08 * cos(4 * PI * i / kernel_length));
        sum += kernel[i];
      }

      for (int i = 0; i < kernel_length; i++) {
        kernel[i] /= sum;
      }
    }

template <typename T_in, typename T_out>
void decimator<T_in, T_out>::process_buffer() {
    int to_read = this -> input_buffer -> available_read();
    if (to_read) {
      int len = decimate(to_read);
      int to_write = this -> output_buffer -> available_write();
      for (int i = 0; i < std::min(len, to_write); i++) {
        this -> output_buffer -> put(decimated_buffer[i]);
      }
    }
}


template <typename T_in, typename T_out>
complex_decimator<T_in, T_out>::complex_decimator(int m_factor, double fc, int kernel_length)
  : decimator<T_in, T_out>(m_factor, fc, kernel_length),
    acc_i {0},
    acc_q {0} {};

template <typename T_in, typename T_out>
int complex_decimator<T_in, T_out>::decimate(int len) {
  int i = 0, j = 0;

  while (i + 2 * this -> kernel_length < len) {
    while (this -> window_cnt < this -> kernel_length) {
      acc_i += this -> input_buffer -> take(i) * this -> kernel[this -> window_cnt];
      acc_q += this -> input_buffer -> take(i + 1) * this -> kernel[this -> window_cnt];
      this -> window_cnt++;
      i += 2;
    }

    this -> decimated_buffer[j] = (T_out)acc_i;
    this -> decimated_buffer[j + 1] = (T_out)acc_q;
    acc_i = acc_q = 0;
    this -> window_cnt = 0;
    i += 2 * (this -> m_factor - this -> kernel_length);
    this -> input_buffer -> advance(2 * this -> m_factor);
    j += 2;
  }

  return j;
};

template <typename T_in, typename T_out>
real_decimator<T_in, T_out>::real_decimator(int m_factor, double fc, int kernel_length)
    : decimator<T_in, T_out>(m_factor, fc, kernel_length),
      acc {0} {};

template <typename T_in, typename T_out>
int real_decimator<T_in, T_out>::decimate(int len) {
  int i = 0, j = 0;

  while (i + this -> kernel_length < len) {
    while (this -> window_cnt < this -> kernel_length) {
      acc += this -> input_buffer -> take(i) * this -> kernel[this -> window_cnt];
      this -> window_cnt++;
      i++;
    }

    this -> decimated_buffer[j] = (T_out)acc;
    acc = 0;
    this -> window_cnt = 0;
    i += this -> m_factor - this -> kernel_length;
    this -> input_buffer -> advance(this -> m_factor);
    j++;
  }

  return j;
};

// These are necessary to avoid linkage error.
template class complex_decimator<int16_t, double>;
template class complex_decimator<double, int16_t>;
template class complex_decimator<double, double>;
template class real_decimator<double, double>;
template class real_decimator<double, int16_t>;
template class real_decimator<int16_t, int16_t>;