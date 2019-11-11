#include <algorithm>
#include <iostream>
#include <cstring>
#include "ring_buffer.h"

/* ring_buffer */

/*
 * Bit magic picked up from https://github.com/JvanKatwijk/sdr-j-fm RingBuffer implementation.
 */
template <class T>
int ring_buffer<T>::available_read()
{
  return (tail - head) & s_mask;
}

/* Moves head forward. offset should not be greater than available_read()!
 * TODO: check it!
 */
template <class T>
void ring_buffer<T>::advance_head(int offset) {
  head = (head + offset) & b_mask;
}

/* Returns element at given offset from head (should be in the buffer).
 * TODO: check it!
 */
template <class T>
T ring_buffer<T>::take(int offset)
{
  return buffer[(head + offset) & b_mask];
}


template <class T>
mem_block<T> ring_buffer<T>::take_block()
{
  if (head <= tail) {
    return { &buffer[head], tail - head };
  } else {
    return { &buffer[head], 2 * size - head };
  }
}

template <class T>
int ring_buffer<T>::available_write()
{
  return size - available_read();
}


/* TODO: check if no overwrite!
 *
 */
template <class T>
void ring_buffer<T>::push_block(mem_block<T> & block)
{
  uint32_t till_end = 2 * size - tail;
  uint32_t to_write = block.length;
  uint32_t available = available_write();
  block.length = (to_write > available) ? available : to_write;

  if (to_write > till_end) {
    std::memcpy(&this->buffer[tail], block.start_index, till_end * sizeof(T));
    std::memcpy(&this->buffer[0], block.start_index + till_end, (to_write - till_end) * sizeof(T));
  } else {
    std::memcpy(&this->buffer[tail], block.start_index, to_write * sizeof(T));
  }
  tail = (tail + to_write) & b_mask;
}

/* TODO: check if no overwrite!
 *
 */
template <class T>
void ring_buffer<T>::push(T element) {
  buffer[tail] = element;
  tail = (tail + 1) & b_mask;
}

// These are necessary to avoid linkage error.
template class ring_buffer<double>;
template class ring_buffer<int16_t>;
