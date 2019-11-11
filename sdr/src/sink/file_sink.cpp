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
    std::unique_lock<std::mutex> l(this->input_buffer->m);
    this->input_buffer->read_lock(l);
    auto b = this->input_buffer->take_block();

    (*target).write(reinterpret_cast<const char *>(b.start_index), b.length * sizeof(T));
    this->input_buffer->move_read_index(b.length);
    this->input_buffer->advance_head();
    this->input_buffer->read_release();
    //std::cerr << "read " << b.length << " at " << this->input_buffer->get_read_offset() << std::endl;
  }
}

// These are necessary to avoid linkage error.
template class file_sink<int16_t>;
template class file_sink<double>;