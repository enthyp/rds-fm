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
T ring_buffer<T, capacity>::take()
{
  if (offset) {
    T el = buffer[head];
    head = (head + 1) % size;
    offset--;
    return el;
  } else {
    throw buffer_empty_exception();
  }
}

template <class T, int capacity>
typename ring_buffer<T, capacity>::block ring_buffer<T, capacity>::take_block()
  throw(buffer_empty_exception)
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