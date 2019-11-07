#include <algorithm>
#include <const.h>
#include "ring_buffer.h"

template <class T, int capacity>
int ring_buffer<T, capacity>::available_read()
{
  if (empty) {
    return 0;
  }
  if (head < tail) {
    return tail - head;
  } else {
    return size - (head - tail);
  }
}

template <class T, int capacity>
int ring_buffer<T, capacity>::available_write()
{
  return size - available_read();
}

template <class T, int capacity>
void ring_buffer<T, capacity>::push(T element)
{
  if (empty || head != tail) {
    buffer[tail] = element;
    tail = (tail + 1) % size;
    empty = false;
  } else {
    throw buffer_full_exception();
  }
}

template <class T, int capacity>
T ring_buffer<T, capacity>::take(int index)
{
  if (!empty) {
    // TODO: illegal access exception?
    return buffer[(head + index) % size];
  } else {
    throw buffer_empty_exception();
  }
}

template <class T, int capacity>
typename ring_buffer<T, capacity>::block ring_buffer<T, capacity>::take_block()
{
  if (!empty) {
    if (head < tail) {
      return { &buffer[head], tail - head };
    } else {
      return { &buffer[head], size - head };
    }
  } else {
    throw buffer_empty_exception();
  }
}

template <class T, int capacity>
void ring_buffer<T, capacity>::advance(int steps)
{
  if (steps > 0) {
    head = (head + steps) % size;
    if (head == tail) {
      empty = true;
    }
  } else {
    throw std::out_of_range("Illegal buffer head operation.");
  }
}

// These are necessary to avoid linkage error.
template class ring_buffer<double, MAXIMUM_BUFFER_LENGTH>;
template class ring_buffer<int16_t, MAXIMUM_BUFFER_LENGTH>;
