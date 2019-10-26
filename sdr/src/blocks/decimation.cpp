#include "blocks/decimation.h"


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
  int i = init_pos;
  int j = 0;

  while (i < buf_size - 1) {
    while (window_cnt < window_len && i < buf_size - 1) {
      acc_i += input_buffer[i];
      acc_q += input_buffer[i + 1];
      window_cnt++;
      i += 2;
    }

    if (window_cnt == window_len) {
      decimated_buffer[j] = acc_i;
      decimated_buffer[j + 1] = acc_q;
      acc_i = acc_q = 0;
      window_cnt = 0;
      j += 2;
    }

    i += 2 * (m_factor - window_len);
  }

  init_pos = i % buf_size;
  return j;
};

void complex_decimator::stop_worker() {
  working = false;
  buf_ready.notify_one();
}