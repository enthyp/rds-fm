#ifndef INCLUDE_RING_BUFFER_H
#define INCLUDE_RING_BUFFER_H

#include <vector>
#include <mutex>
#include <stdexcept>
#include <condition_variable>
#include "const.h"


template <class T>
struct mem_block {
  T * start_index;
  uint32_t length;
};


template <class T>
class ring_buffer {
 private:
  std::mutex m;
  std::condition_variable read_v;
  bool read_c;

  uint32_t head, tail, size, s_mask, b_mask;
  std::vector<T> buffer;

 public:
  ring_buffer()
    : buffer (2 * MAXIMUM_BUFFER_LENGTH, 0),
      head {0},
      tail {0},
      s_mask {MAXIMUM_BUFFER_LENGTH - 1},
      b_mask {2 * MAXIMUM_BUFFER_LENGTH - 1},
      size {MAXIMUM_BUFFER_LENGTH},
      read_c {false} {};

  /* Buffer access synchronization */
  class signal_lock {
   protected:
    bool & cond;
    std::condition_variable & cv;
    std::unique_lock<std::mutex> lock;
   public:
    signal_lock(std::mutex & m, std::condition_variable & cv, bool & cond);
    signal_lock(signal_lock && other);
    virtual ~signal_lock() = 0;
  };
  class r_lock : signal_lock {
   public:
    r_lock(std::mutex & m, std::condition_variable & cv, bool & cond);
    r_lock(r_lock && other);
    ~r_lock();
  };
  class w_lock : signal_lock {
   public:
    w_lock(std::mutex & m, std::condition_variable & cv, bool & cond);
    w_lock(w_lock &&other);
    ~w_lock();
  };

  r_lock read_lock();
  w_lock write_lock();

  /* Buffer data access */
  int available_read();
  void advance_head(int offset);
  T take(int offset) noexcept (false);
  mem_block<T> take_block() noexcept (false);

  int available_write();
  void push_block(mem_block<T> & block) noexcept (false);
  void push(T element) noexcept (false);
};

#endif  /* INCLUDE_RING_BUFFER_H */
