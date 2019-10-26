#ifndef BLOCKS_SOURCE
#define BLOCKS_SOURCE

#include <thread>

#include "block.h"


class source : public producer {
 protected:
  std::shared_ptr<consumer> consumer;
  virtual void produce() = 0;
  std::thread worker_t;
  virtual void stop_worker() = 0;

 public:
  virtual void rtl_source::run() {
    worker_t = std::thread(produce);
  }

  virtual void stop()
  {
    stop_worker();
    worker_t.join();
  }

  virtual void to(std::shared_ptr<consumer> b)
  {
    consumer = b;
  }
};

#endif  /* BLOCKS_SOURCE */
