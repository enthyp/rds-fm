#include <algorithm>
#include "const.h"
#include "ring_buffer.h"

/* signal_lock */

template <class T, int capacity>
ring_buffer<T, capacity>::signal_lock::signal_lock(
    std::mutex & m,
    std::condition_variable & init_cond,
    bool & init_flag,
    std::condition_variable & final_cond,
    bool & final_flag)
    : lck (m),
      init_cond {init_cond},
      final_cond {final_cond},
      init_flag {init_flag},
      final_flag {final_flag}
      {
        init_cond.wait(lck, [this] { return this -> init_flag; });
      };


template <class T, int capacity>
ring_buffer<T, capacity>::signal_lock::~signal_lock() {
  final_flag = true;
  init_flag = false;
  final_cond.notify_all();
}

template<class T, int capacity>
ring_buffer<T, capacity>::signal_lock::signal_lock(ring_buffer<T, capacity>::signal_lock && other)
  : lck(std::move(other.lck)),
    init_cond {other.init_cond},
    init_flag {other.init_flag},
    final_cond {other.final_cond},
    final_flag {other.final_flag} {};



/* ring_buffer */

template <class T, int capacity>
typename ring_buffer<T, capacity>::signal_lock ring_buffer<T, capacity>::read_lock()
{
  ring_buffer<T, capacity>::signal_lock l(m, read_v, read_c, write_v, write_c);
  return l;
}

template <class T, int capacity>
typename ring_buffer<T, capacity>::signal_lock ring_buffer<T, capacity>::write_lock()
{
  ring_buffer<T, capacity>::signal_lock l(m, write_v, write_c, read_v, read_c);
  return l;
}

template <class T, int capacity>
int ring_buffer<T, capacity>::available_read()
{
  if (head <= tail) {
    return tail - head;
  } else {
    return size - (head - tail);
  }
}

template <class T, int capacity>
int ring_buffer<T, capacity>::available_write()
{
  return capacity - available_read();
}

template <class T, int capacity>
void ring_buffer<T, capacity>::push(T element)
{
  if (head == tail + 1) {
    throw buffer_full_exception();
  }

  buffer[tail] = element;
  tail = (tail + 1) % size;
}

template <class T, int capacity>
T ring_buffer<T, capacity>::take(int index)
{
  if (head == tail) {
    throw buffer_empty_exception();
  }

  // TODO: illegal access exception?
  return buffer[(head + index) % size];
}

template <class T, int capacity>
typename ring_buffer<T, capacity>::block ring_buffer<T, capacity>::take_block()
{
  if (head == tail) {
    throw buffer_empty_exception();
  }

  if (head < tail) {
    return { &buffer[head], tail - head };
  } else {
    return { &buffer[head], size - head };
  }

}

template <class T, int capacity>
void ring_buffer<T, capacity>::advance(int steps)
{
  if (steps > 0 && steps <= available_read()) {
    head = (head + steps) % size;
  } else {
    throw std::out_of_range("Illegal buffer head operation.");
  }
}

// These are necessary to avoid linkage error.
template class ring_buffer<double, MAXIMUM_BUFFER_LENGTH>;
template class ring_buffer<int16_t, MAXIMUM_BUFFER_LENGTH>;

