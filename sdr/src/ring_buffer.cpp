#include "ring_buffer.h"


template <class T, int capacity>
void ring_buffer<T, capacity>::push(T el) {
  if (empty || offset != 0) {
    int tail = (head + offset) & size;
    buffer[tail] = el;
    offset++;
    empty = false;
  } else {
    throw buffer_full_exception();
  }
}

template <class T, int capacity>
T ring_buffer<T, capacity>::take() {
  if (!empty) {
    T el = buffer[head];
    head = (head + 1) % size;
    empty = (--offset == 0);
  } else {
    throw buffer_empty_exception();
  }
}

template <class T, int capacity>
int ring_buffer<T, capacity>::available_read() {
  return offset;
}

template <class T, int capacity>
int ring_buffer<T, capacity>::available_write() {
  return size - offset;
}