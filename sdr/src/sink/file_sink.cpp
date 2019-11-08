#include <iostream>
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
void file_sink<T>::worker() {
  int i = 0;
  while (working) {
    auto lock = this -> input_buffer -> read_lock();
    auto b = this -> input_buffer -> take_block();

    (*target).write(reinterpret_cast<const char *>(b.start_index), b.length * sizeof(T));
    consumer<T>::input_buffer -> advance(b.length);
  }
}

// These are necessary to avoid linkage error.
template class file_sink<int16_t>;
template class file_sink<double>;