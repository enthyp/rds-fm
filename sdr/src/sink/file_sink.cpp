#include <iostream>
#include <mutex>

#include "sink/file_sink.h"
#include "ring_buffer.h"


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
  while (working) {
    try {
      typename ring_buffer<T, MAXIMUM_BUFFER_LENGTH>::block b = consumer<T>::input_buffer -> take_block();
      (*target).write(reinterpret_cast<const char *>(b.start_index), b.length * sizeof(T));
      consumer<T>::input_buffer -> advance(b.length);
    } catch(buffer_empty_exception & e) {
      // Do nothing, a busy loop for now.
    }
  }
}

// These are necessary to avoid linkage error.
template class file_sink<int16_t>;
template class file_sink<double>;