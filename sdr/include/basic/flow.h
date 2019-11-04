#ifndef BLOCKS_FLOW_H
#define BLOCKS_FLOW_H

#include <iostream>
#include <cstring>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "basic/block.h"
#include "ring_buffer.h"


// TODO: I guess we could get rid off templates for now?
template <typename T_in, typename T_out>
class flow : public producer<T_out>, public consumer<T_in> {
 protected:
  std::shared_ptr<consumer<T_out>> succ;

  ring_buffer<T_in> output_buffer;

  virtual void process() = 0;
  std::thread worker_t;
  virtual void stop_worker() = 0;

 public:
  flow() : output_buffer {0} {};
  void run() override = 0;
  void stop() override
  {
    stop_worker();
    worker_t.join();
  }

  void receive(T_in * buffer, int len) override
  {
    std::unique_lock<std::mutex> lock(buf_lock);
    std::memcpy(output_buffer + offset, buffer, sizeof(T_in) * len);
    buf_size = len + offset;
    // TODO: this memcpy is a waste of time - the source buffer pointer + len are enough!!!
    // So we could either have input buffer hard linked or have a reference to start passed every time here.
    read_ready = true;
    lock.unlock();
    read_ready_cond.notify_one();
  }

  void to(std::shared_ptr<consumer<T_out>> c) override
  {
    succ = c;
  }
};


#endif  /* BLOCKS_FLOW_H */
