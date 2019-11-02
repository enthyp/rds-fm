#include <iostream>
#include <mutex>

#include "out/file_sink.h"

template <typename T>
file_sink<T>::file_sink(std::string & filename)
  : sink<T>(), working {false}
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
void file_sink<T>::consume() {
  int count = 0;
  while (working) {
    // Wait for data to appear in the buffer and save it to file.
    std::unique_lock<std::mutex> lock(sink<T>::buf_lock);
    if (!sink<T>::read_ready) {
      sink<T>::read_ready_cond.wait(lock);
    }

    if (!working)
      break;

    count += sink<T>::buf_size;
    (*target).write(reinterpret_cast<const char *>(sink<T>::input_buffer), sink<T>::buf_size * sizeof(T));

    sink<T>::write_ready = true;
    sink<T>::read_ready = false;
    lock.unlock();
    sink<T>::write_ready_cond.notify_one();
  }

  std::cerr << "sink " << count << std::endl;
}

template <typename T>
void file_sink<T>::stop_worker() {
  working = false;
  sink<T>::read_ready_cond.notify_one();
}

// These are necessary to avoid linkage error.
template class file_sink<int16_t>;
template class file_sink<double>;