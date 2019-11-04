#include "ring_buffer.h"


template <class T, int capacity>
void ring_buffer<T, capacity>::push(T el) {
  if (empty || head != tail) {
    buffer[tail] = el;
    tail = (tail + 1) % size;
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
    empty = (head == tail);
  } else {
    throw buffer_empty_exception();
  }
}
