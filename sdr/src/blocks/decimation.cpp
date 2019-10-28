#include <iostream>
#include "const.h"
#include "blocks/decimation.h"


complex_decimator::complex_decimator(int m_factor, double fc, int kernel_length)
  : flow(),
    m_factor {m_factor},
    fc {fc},
    kernel_length {kernel_length},
    kernel (kernel_length, 0.),
    decimated_buffer {0},
    working {false},
    acc_i {0.},
    acc_q {0.},
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

void complex_decimator::process() {
  while (working) {
    // Wait for data in the buffer.
    std::unique_lock<std::mutex> lock(buf_lock);
    buf_ready.wait(lock);

    if (!working)
      break;

    int len = decimate();

    // And send the data to output block.
    succ -> receive(decimated_buffer, len);
  }
}

int complex_decimator::decimate() {
  int i = 0, j = 0;

  while (i + 2 * kernel_length < buf_size) {
    while (window_cnt < kernel_length) {
      acc_i += input_buffer[i] * kernel[window_cnt];
      acc_q += input_buffer[i + 1] * kernel[window_cnt];
      window_cnt++;
      i += 2;
    }

    decimated_buffer[j] = (int32_t)acc_i;
    decimated_buffer[j + 1] = (int32_t)acc_q;
    acc_i = acc_q = 0;
    window_cnt = 0;
    i += 2 * (m_factor - kernel_length);
    j += 2;
  }

  if (i < buf_size) {
    std::memcpy(input_buffer, input_buffer + i, (buf_size - i + 1) * sizeof(int16_t));
    offset = buf_size - i + 1;
  } else {
    offset = 0;
  }

  return j;
};

void complex_decimator::stop_worker() {
  working = false;
  buf_ready.notify_one();
}