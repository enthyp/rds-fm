#include <algorithm>
#include <iostream>
#include "ring_buffer.h"


/* signal_lock */

template <class T>
ring_buffer<T>::signal_lock::signal_lock(
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


template <class T>
ring_buffer<T>::signal_lock::~signal_lock() {
  final_flag = true;
  init_flag = false;
  final_cond.notify_all();
}

template<class T>
ring_buffer<T>::signal_lock::signal_lock(ring_buffer<T>::signal_lock && other)
  : lck(std::move(other.lck)),
    init_cond {other.init_cond},
    init_flag {other.init_flag},
    final_cond {other.final_cond},
    final_flag {other.final_flag} {};


/* ring_buffer */

template <class T>
int ring_buffer<T>::get_size() { return size; }

/* Returns read index offset relative to head position. It is always non-negative and might formally
 * surpass tail (think decimation filters).
 */
template <class T>
int ring_buffer<T>::get_read_offset() {
  return read_offset;
}

//template <class T>
//typename ring_buffer<T>::signal_lock ring_buffer<T>::read_lock()
//{
//  ring_buffer<T>::signal_lock l(m, read_v, read_c, write_v, write_c);
//  return l;
//}
//
//template <class T>
//typename ring_buffer<T>::signal_lock ring_buffer<T>::write_lock()
//{
//  ring_buffer<T>::signal_lock l(m, write_v, write_c, read_v, read_c);
//  return l;
//}


template <class T>
void ring_buffer<T>::read_release() {
  write_c = true;
  write_v.notify_one();
  read_c = false;
}


template <class T>
void ring_buffer<T>::write_release() {
  read_c = true;
  read_v.notify_one();
  write_c = false;
}

template <class T>
int ring_buffer<T>::available_read()
{
  if (head < tail) {
    return tail - head;
  }
  if (empty) {
    return 0;
  } else {
    return size - (head - tail);
  }
}

/* Advances read index of the buffer - it might surpass tail of the buffer
 * and reads become unavailable until tail in turn surpasses read index.
 * Should only be called with non-negative offsets or undefined messy behaviour
 * will occur.
 */
template <class T>
void ring_buffer<T>::move_read_index(int offset) {
  read_offset += offset;
}

/* Moves head forward to where read index currently is. No other movements of
 * buffer head are allowed.
 */
template <class T>
void ring_buffer<T>::advance_head() {
  int av_read = available_read();
  head = (head + read_offset) % size;

  if (read_offset >= av_read) {
    tail = head;
    empty = true;
  }

  read_offset = 0;
}

/* Returns element at read index + offset (it it's in the buffer).
 * Offset should be non-negative.
 */
template <class T>
T ring_buffer<T>::take(int offset)
{
  return buffer[(head + read_offset + offset) % size];
}


template <class T>
typename ring_buffer<T>::block ring_buffer<T>::take_block()
{
  if ((head + read_offset) % size < tail) {
    return { &buffer[(head + read_offset) % size], tail - head - read_offset };
  } else {
    return { &buffer[(head + read_offset) % size], size - head - read_offset};
  }
}

template <class T>
int ring_buffer<T>::available_write()
{
  return size - available_read();
}

template <class T>
void ring_buffer<T>::push(T element)
{
  buffer[tail] = element;
  tail = (tail + 1) % size;
  empty = false;
}

// These are necessary to avoid linkage error.
template class ring_buffer<double>;
template class ring_buffer<int16_t>;
