#ifndef RECEIVER_H
#define RECEIVER_H

#include "base.h"


template <class T>
class pipeline {
private:
  std::vector<ring_buffer<T, MAXIMUM_BUFFER_LENGTH>> links;
  std::vector<block*> blocks;
  void link(producer<T> *, consumer<T> *);

public:
  explicit pipeline(producer<T> source) : blocks {source} {};
  void add(consumer<T>* b);
  void run();
  void stop();
};

#endif  /* RECEIVER_H */