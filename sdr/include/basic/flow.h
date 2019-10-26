#ifndef BLOCKS_FLOW_H
#define BLOCKS_FLOW_H

#include <cstring>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "basic/block.h"


class flow : public producer, public consumer {
 protected:
  std::shared_ptr<consumer> succ;

  int16_t input_buffer[MAXIMUM_BUFFER_LENGTH];
  uint32_t buf_size;
  int offset;
  std::mutex buf_lock;
  std::condition_variable buf_ready;

  virtual void process() = 0;
  std::thread worker_t;
  virtual void stop_worker() = 0;

 public:
  flow() : offset {0}, buf_size {0}, input_buffer {0} {};
  void run() override = 0;
  void stop() override
  {
    stop_worker();
    worker_t.join();
  }

  void receive(int16_t * buffer, int len) override
  {
    std::lock_guard<std::mutex> lock(buf_lock);
    std::memcpy(input_buffer + offset, buffer, sizeof(int16_t) * len);
    buf_size = len + offset;
    buf_ready.notify_one();
  }

  void to(std::shared_ptr<consumer> c) override
  {
    succ = c;
  }
};


#endif  /* BLOCKS_FLOW_H */
