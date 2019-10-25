#ifndef BLOCKS_SOURCE
#define BLOCKS_SOURCE

#include "block.h"
#include "sink.h"


class source : public producer {
 protected:
  std::shared_ptr<consumer> consumer;
  std::unique_ptr<task> produce;
  std::thread worker_t;

 public:
  virtual void run()
  {
    worker_t = std::thread(std::ref(*produce));
  }

  virtual void stop()
  {
    this -> produce -> stop();
    this -> worker_t.join();
  }

  virtual void to(std::shared_ptr<consumer> b)
  {
    consumer = b;
  }
};

#endif  /* BLOCKS_SOURCE */
