#include <iostream>
#include <algorithm>
#include "source/file_source.h"


void file_source::worker() {
  while (working && source_file.read(reinterpret_cast<char *>(im_buffer), DEFAULT_BUFFER_LENGTH * sizeof(int16_t))) {
    auto lock = output_buffer->write_lock();

    unsigned long available = output_buffer->available_write();
    unsigned long count = source_file.gcount() / sizeof(int16_t);

    for (unsigned long i = 0; i < std::min(count, available); i++) {
      output_buffer->push(im_buffer[i]);
    }
  }

  std::cerr << "Done! " << std::endl;
}
