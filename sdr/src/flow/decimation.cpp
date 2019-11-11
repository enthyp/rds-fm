#include <iostream>
#include <algorithm>
#include "const.h"
#include "flow/decimation.h"


template <typename T_in, typename T_out>
decimator<T_in, T_out>::decimator(int m_factor, double fc, int kernel_length)
  : flow<T_in, T_out>(),
    m_factor {m_factor},
    fc {fc},
    kernel_length {kernel_length},
    kernel (kernel_length, 0.),
    decimated_buffer {0},
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
  uint32_t len;
  {
    std::unique_lock<std::mutex> l(this->input_buffer->m);
    this->input_buffer->read_lock(l);
    int to_read = this->input_buffer->available_read();
    len = decimate(to_read);
    this->input_buffer->read_release();
  }

  auto lock = this->output_buffer->write_lock();
  mem_block<T_out> b = {decimated_buffer, len};
  this->output_buffer->push_block(b);
}

template <typename T_in, typename T_out>
complex_decimator<T_in, T_out>::complex_decimator(int m_factor, double fc, int kernel_length)
  : decimator<T_in, T_out>(m_factor, fc, kernel_length),
    acc_i {0},
    acc_q {0} {};

template <typename T_in, typename T_out>
uint32_t complex_decimator<T_in, T_out>::decimate(int len) {
  int i = 0;

  while (len >= 2 * (this->kernel_length - this->window_cnt) && len > 2 * this->m_factor) {
    int j = 0;
    while (this->window_cnt < this->kernel_length) {
      acc_i += this->input_buffer->take(j) * this->kernel[this->window_cnt];
      acc_q += this->input_buffer->take(j + 1) * this->kernel[this->window_cnt];
      this->window_cnt++;
      j += 2;
    }

    this->decimated_buffer[i] = (T_out) acc_i;
    this->decimated_buffer[i + 1] = (T_out) acc_q;
    i += 2;

    this->window_cnt = acc_i = acc_q = 0;
    this->input_buffer->advance_head(2 * this->m_factor);
    len -= 2 * this->m_factor;
  }

  return i;
};

template <typename T_in, typename T_out>
real_decimator<T_in, T_out>::real_decimator(int m_factor, double fc, int kernel_length)
    : decimator<T_in, T_out>(m_factor, fc, kernel_length),
      acc {0} {};

template <typename T_in, typename T_out>
uint32_t real_decimator<T_in, T_out>::decimate(int len) {
  int i = 0;

  while (len >= this->kernel_length - this->window_cnt && len > this->m_factor) {
    int j = 0;
    while (this->window_cnt < this->kernel_length) {
      acc += this->input_buffer->take(j) * this->kernel[this->window_cnt];
      this->window_cnt++;
      j++;
    }

    this->decimated_buffer[i] = (T_out) acc;
    i++;

    this->window_cnt = acc = 0;
    this->input_buffer->advance_head(this->m_factor);
    len -= this->m_factor;
  }

  return i;
};

// These are necessary to avoid linkage error.
template class complex_decimator<int16_t, int16_t>;
template class complex_decimator<int16_t, double>;
template class complex_decimator<double, double>;
template class real_decimator<double, double>;
template class real_decimator<int16_t, int16_t>;