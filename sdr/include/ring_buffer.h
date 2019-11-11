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
class ring_buffer {
 private:
  bool read_c;
  std::condition_variable read_v;

  bool empty;
  int head, tail, read_offset, size;
  std::vector<T> buffer;

 public:
  std::mutex m;

  ring_buffer()
    : buffer (MAXIMUM_BUFFER_LENGTH, 0),
      head {0},
      tail {0},
      read_offset {0},
      empty {true},
      size {MAXIMUM_BUFFER_LENGTH},
      read_c {false} {};

  int get_size();
  int get_read_offset();

  /* Buffer access synchronization */

  class signal_lock {
   private:
    std::unique_lock<std::mutex> lck;
    std::condition_variable & final_cond;
    bool & final_flag;

   public:
    signal_lock(
        std::mutex & m,
        std::condition_variable & final_cond,
        bool & final_flag);
    ~signal_lock();
    signal_lock(signal_lock && lock);
  };

  void read_lock(std::unique_lock<std::mutex> & m);
  void read_release();
  signal_lock write_lock();


  /* Buffer data access */

  struct block {
    T * start_index;
    int length;
  };

  int available_read();
  void move_read_index(int offset);
  void advance_head();
  T take(int offset) noexcept (false);
  block take_block() noexcept (false);

  int available_write();
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
