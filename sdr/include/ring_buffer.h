#ifndef INCLUDE_RING_BUFFER_H
#define INCLUDE_RING_BUFFER_H

#include <vector>
#include <stdexcept>
#include <mutex>
#include <condition_variable>


template <class T, int capacity>
class ring_buffer {
 private:
  std::vector<T> buffer;
  int head, tail, size;
  bool empty;
  std::mutex lock;
  std::condition_variable ready_cond;
  bool ready;

 public:
  ring_buffer(int size)
    : buffer (size, capacity),
      head {0},
      tail {0},
      size {size},
      empty {true} {};
  void push(T el);
  T take();
};
// TODO: how to balance reads and writes?
// 1: no mutex - writes have to wait until reads are done (maybe move by blocks of 16384)
// 2: with mutex - writes can overwrite unread bits (not a ring buffer per se)
//    justification: we only need to process the most fresh data I guess...


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
