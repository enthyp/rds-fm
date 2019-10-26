#ifndef BLOCKS_SOURCE_H
#define BLOCKS_SOURCE_H

#include <thread>
#include "basic/block.h"


class source : public producer {
 protected:
  std::shared_ptr<consumer> succ;
  virtual void produce() = 0;
  std::thread worker_t;
  virtual void stop_worker() = 0;

 public:
  void run() override = 0;
  void stop() override
  {
    stop_worker();
    worker_t.join();
  }

  void to(std::shared_ptr<consumer> c) override
  {
    succ = c;
  }
};

#endif  /* BLOCKS_SOURCE_H */
