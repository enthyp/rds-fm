#include <iostream>
#include "pipeline.h"

template <class T>
void pipeline<T>::add(consumer<T> *b) {
  if (!blocks.empty()) {
    link(static_cast<producer<T>*>(*blocks.end()), b);
  }
  blocks.push_back(b);
}

template <class T>
void pipeline<T>::link(producer<T> *p, consumer<T> *c) {
  ring_buffer<T, MAXIMUM_BUFFER_LENGTH> buffer = ring_buffer<T, MAXIMUM_BUFFER_LENGTH>();
  p -> to(buffer);
  c -> from(buffer);
  links.push_back(buffer);
}

template <typename T>
void pipeline<T>::run() {
  for (auto it = blocks.end(); it != blocks.begin(); it--) {
    (**it).run();
  }

  std::cerr << "Running..." << std::endl;
}

template <typename T>
void pipeline<T>::stop() {
  for (auto it = blocks.begin(); it != blocks.end(); it--) {
    (**it).run();
  }

  std::cerr << "Stopped." << std::endl;
}

// These are necessary to avoid linkage error.
template class pipeline<double>;
