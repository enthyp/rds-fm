#include <iostream>
#include "const.h"
#include "blocks/decimation.h"

template <typename T_in, typename T_out>
decimator<T_in, T_out>::decimator(int m_factor, double fc, int kernel_length)
  : flow<T_in, T_out>(),
    m_factor {m_factor},
    fc {fc},
    kernel_length {kernel_length},
    kernel (kernel_length, 0.),
    decimated_buffer {0},
    working {false},
    window_cnt {0}
    {
      double sum = 0;

      // Windowed-sinc filter (Blackman window).
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
void decimator<T_in, T_out>::process() {
  int count = 0;
  while (working) {
    // Wait for data in the buffer.
    std::unique_lock<std::mutex> lock(flow<T_in, T_out>::buf_lock);
    if (!flow<T_in, T_out>::read_ready) {
      flow<T_in, T_out>::read_ready_cond.wait(lock, [this] { return this -> read_ready; });
    }

    if (!working)
      break;

    int len = decimate();
    count += len;
    // And send the data to output block.
    flow<T_in, T_out>::succ -> receive(decimated_buffer, len);

    flow<T_in, T_out>::read_ready = false;
    lock.unlock();
  }
}

template <typename T_in, typename T_out>
void decimator<T_in, T_out>::stop_worker() {
  working = false;
  this -> read_ready = true;
  flow<T_in, T_out>::read_ready_cond.notify_one();
}

template <typename T_in, typename T_out>
complex_decimator<T_in, T_out>::complex_decimator(int m_factor, double fc, int kernel_length)
  : decimator<T_in, T_out>(m_factor, fc, kernel_length),
    acc_i {0},
    acc_q {0} {};

template <typename T_in, typename T_out>
int complex_decimator<T_in, T_out>::decimate() {
  int i = 0, j = 0;

  while (i + 2 * this -> kernel_length < flow<T_in, T_out>::buf_size) {
    while (this -> window_cnt < this -> kernel_length) {
      acc_i += flow<T_in, T_out>::input_buffer[i] * this -> kernel[this -> window_cnt];
      acc_q += flow<T_in, T_out>::input_buffer[i + 1] * this -> kernel[this -> window_cnt];
      this -> window_cnt++;
      i += 2;
    }

    this -> decimated_buffer[j] = (T_out)acc_i;
    this -> decimated_buffer[j + 1] = (T_out)acc_q;
    acc_i = acc_q = 0;
    this -> window_cnt = 0;
    i += 2 * (this -> m_factor - this -> kernel_length);
    j += 2;
  }

  if (i < this -> buf_size) {
    std::memcpy(
        this -> input_buffer,
        this -> input_buffer + i,
        (this -> buf_size - i) * sizeof(T_in));
    this -> offset = this -> buf_size - i;
  } else {
    this -> offset = 0;
  }

  return j;
};

template <typename T_in, typename T_out>
real_decimator<T_in, T_out>::real_decimator(int m_factor, double fc, int kernel_length)
    : decimator<T_in, T_out>(m_factor, fc, kernel_length),
      acc {0} {};

template <typename T_in, typename T_out>
int real_decimator<T_in, T_out>::decimate() {
  int i = 0, j = 0;

  while (i + this -> kernel_length < this -> buf_size) {
    while (this -> window_cnt < this -> kernel_length) {
      acc += this -> input_buffer[i] * this -> kernel[this -> window_cnt];
      this -> window_cnt++;
      i++;
    }

    this -> decimated_buffer[j] = (T_out)acc;
    acc = 0;
    this -> window_cnt = 0;
    i += this -> m_factor - this -> kernel_length;
    j++;
  }

  if (i < this -> buf_size) {
    std::memcpy(
        this -> input_buffer,
        this -> input_buffer + i,
        (this -> buf_size - i) * sizeof(T_in));
    this -> offset = this -> buf_size - i;
  } else {
    this -> offset = 0;
  }

  return j;
};

// These are necessary to avoid linkage error.
template class complex_decimator<int16_t, double>;
template class complex_decimator<double, int16_t>;
template class complex_decimator<double, double>;
template class real_decimator<double, double>;
template class real_decimator<int16_t, int16_t>;