#ifndef INCLUDE_RING_BUFFER_H
#define INCLUDE_RING_BUFFER_H

#include <vector>
#include <mutex>
#include <stdexcept>
#include <condition_variable>
#include "const.h"


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


template <class T>
struct mem_block {
  T * start_index;
  uint32_t length;
};


template <class T>
class ring_buffer {
 private:
  uint32_t head, tail, size, s_mask, b_mask;
  std::vector<T> buffer;

 public:
  std::mutex m;
  std::condition_variable read_v;
  bool read_c;

  ring_buffer()
    : buffer (2 * MAXIMUM_BUFFER_LENGTH, 0),
      head {0},
      tail {0},
      s_mask {MAXIMUM_BUFFER_LENGTH - 1},
      b_mask {2 * MAXIMUM_BUFFER_LENGTH - 1},
      size {MAXIMUM_BUFFER_LENGTH},
      read_c {false} {};

  /* Buffer data access */
  int available_read();
  void advance_head(int offset);
  T take(int offset) noexcept (false);
  mem_block<T> take_block() noexcept (false);

  int available_write();
  void push_block(mem_block<T> & block) noexcept (false);
  void push(T element) noexcept (false);
};

// TODO: how to balance reads and writes?
// Writer:
// 1: let writers overwrite
// 2: reject excessive writes
// 3: wait for free space to write to
//
// Reader:
// 1. suspend reader when no data available (cond variable)
// 2. busy wait source reader
// changed my mind: my processor does not seem to like them busy waits. Hence
// FOR NOW: (2, 1)

#endif  /* INCLUDE_RING_BUFFER_H */
