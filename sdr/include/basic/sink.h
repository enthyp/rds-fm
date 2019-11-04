#ifndef BLOCKS_SINK
#define BLOCKS_SINK

#include <atomic>
#include <cstring>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "block.h"


template <typename T>
class sink : public consumer<T> {
 protected:
  virtual void consume() = 0;

 public:
  void run() override = 0;
};

#endif  /* BLOCKS_SINK */
