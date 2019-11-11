#include <iostream>
#include <algorithm>
#include <unistd.h>
#include "source/file_source.h"


void file_source::worker() {
  while (working && source_file.read(reinterpret_cast<char *>(im_buffer), DEFAULT_BUFFER_LENGTH * sizeof(int16_t))) {
    usleep(100);  // don't make aplay choke with too high sample rate
    std::unique_lock<std::mutex> lock(this->output_buffer->m);

    uint32_t count = source_file.gcount() / sizeof(int16_t);
    mem_block<int16_t> b = {im_buffer, count};
    output_buffer->push_block(b);

    this->output_buffer->read_c = true;
    lock.unlock();
    this->output_buffer->read_v.notify_one();
  }

  std::cerr << "Done! " << std::endl;
}
