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
  head = (head + steps) % size;
}