#include <cstring>
#include <iostream>
#include <fstream>
#include <memory>
#include <mutex>

#include "rtl-sdr.h"
#include "out/file_sink.h"


file_sink::file_sink(std::string & filename)
  : working {false}
  {
    // Open the output stream.
    if (filename != "-") {
      out_file.open(filename, std::ios::binary | std::ios::out);
      target = &out_file;
    } else {
      target = &(std::cout);
    }
  }

void file_sink::consume() {
  while (working) {
    // Wait for data to appear in the buffer and save it to file.
    std::unique_lock<std::mutex> lock(buf_lock);
    buf_ready.wait(lock);

    if (!working)
      break;

    (*target).write(reinterpret_cast<const char *>(input_buffer), buf_size * sizeof(int16_t));
  }
}

void file_sink::stop_worker() {
  working = false;
  buf_ready.notify_one();
}