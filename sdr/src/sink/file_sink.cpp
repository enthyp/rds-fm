#include <iostream>
#include <mutex>

#include "sink/file_sink.h"

template <typename T>
file_sink<T>::file_sink(std::string & filename)
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

template <typename T>
void file_sink<T>::work() {
  int count = 0;
  while (working) {
    (*target).write(reinterpret_cast<const char *>(sink<T>::input_buffer), sink<T>::buf_size * sizeof(T));
  }
}

// These are necessary to avoid linkage error.
template class file_sink<int16_t>;
template class file_sink<double>;