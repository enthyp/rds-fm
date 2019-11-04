#ifndef INCLUDE_RING_BUFFER_H
#define INCLUDE_RING_BUFFER_H

#include <vector>
#include <stdexcept>


template <class T, int capacity>
class ring_buffer {
 private:
  std::vector<T> buffer;
  int head, offset, size;
  bool empty;

 public:
  ring_buffer(int size)
    : buffer (size, capacity),
      head {0},
      offset {0},
      size {size},
      empty {true} {};
  void push(T el);
  T take();
  int available_write();
  int available_read();
};

// TODO: how to balance reads and writes?
// Writer:
// 1: let writers overwrite
// 2: reject excessive writes
// 3: reject excessive writes
//
// Reader:
// 1. suspend reader when no data available (cond variable)
// 2. busy wait in reader
// FOR NOW: reject excessive writes (easy), busy wait in reader (should rarely happen)


class buffer_full_exception : public std::runtime_error {
 public:
  buffer_full_exception() :
      std::runtime_error("Buffer full!") {};
};

class buffer_empty_exception : public std::runtime_error {
 public:
  buffer_empty_exception() :
      std::runtime_error("Buffer empty!") {};
};

#endif  /* INCLUDE_RING_BUFFER_H */
