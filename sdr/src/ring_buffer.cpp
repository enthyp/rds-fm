#include <const.h>
#include "ring_buffer.h"


template <class T, int capacity>
void ring_buffer<T, capacity>::push(T element)
{
  if (offset < size) {
    buffer[(head + offset++) % size] = element;
  } else {
    throw buffer_full_exception();
  }
}

template <class T, int capacity>
T ring_buffer<T, capacity>::take(int index)
{
  if (index >= 0 && index < offset) {
    return buffer[head + index];
  } else {
    throw std::out_of_range("Illegal buffer access.");
  }
}

template <class T, int capacity>
typename ring_buffer<T, capacity>::block ring_buffer<T, capacity>::take_block()
{
  if (head + offset > size) {
    // We have 2 blocks to return (on both ends of the buffer).
    return { &buffer[head], size - head };
  } else if (offset > 0) {
    // Just one block to return.
    return { &buffer[head], offset };
  } else {
    throw buffer_empty_exception();
  }
}

template <class T, int capacity>
void ring_buffer<T, capacity>::advance(int steps)
{
  if (steps <= offset) {
    head = (head + steps) % size;
  } else {
    throw std::out_of_range("Illegal buffer head operation.");
  }
}

// These are necessary to avoid linkage error.
template class ring_buffer<double, MAXIMUM_BUFFER_LENGTH>;
template class ring_buffer<int16_t, MAXIMUM_BUFFER_LENGTH>;
