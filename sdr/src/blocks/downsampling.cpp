#include "blocks/downsampling.h"


void downsampler::process() {
  while (working) {
    // Wait for data in the buffer.
    std::unique_lock<std::mutex> lock(buf_lock);
    buf_ready.wait(lock);

    if (!working)
      break;

    int len = downsample();

    // And send the data to output block.
    succ -> receive(downsampled_buffer, len);
  }
}

int downsampler::downsample() {
  int i = pos;
  int j = 0;

  while (i < buf_size) {
    downsampled_buffer[j] = input_buffer[i];
    i += m_factor;
    j++;
  }

  pos = i % buf_size;
  return j;
}

void downsampler::stop_worker() {
  working = false;
  buf_ready.notify_one();
}