#include <iostream>
#include <algorithm>
#include "source/file_source.h"


void file_source::worker() {
  while (working && source_file.read(reinterpret_cast<char *>(im_buffer), DEFAULT_BUFFER_LENGTH * sizeof(int16_t))) {
    auto lock = output_buffer->write_lock();

    uint32_t count = source_file.gcount() / sizeof(int16_t);
    mem_block<int16_t> b = {im_buffer, count};
    output_buffer->push_block(b);
  }

  std::cerr << "Done! " << std::endl;
}
