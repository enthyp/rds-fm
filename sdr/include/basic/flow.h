#ifndef BLOCKS_FLOW_H
#define BLOCKS_FLOW_H

#include <iostream>
#include <cstring>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "basic/block.h"


template <typename T_in, typename T_out>
class flow : public producer<T_out>, public consumer<T_in> {
 protected:
  std::shared_ptr<consumer<T_out>> succ;

  T_in input_buffer[MAXIMUM_BUFFER_LENGTH];
  int buf_size;
  int offset;
  std::mutex buf_lock;
  std::condition_variable read_ready_cond;
  bool read_ready;
  std::condition_variable write_ready_cond;
  bool write_ready;

  virtual void process() = 0;
  std::thread worker_t;
  virtual void stop_worker() = 0;

 public:
  flow() : offset {0}, buf_size {0}, input_buffer {0}, read_ready {false}, write_ready {true} {};
  void run() override = 0;
  void stop() override
  {
    stop_worker();
    worker_t.join();
  }

  void receive(T_in * buffer, int len) override
  {
    std::unique_lock<std::mutex> lock(buf_lock);
    if (!write_ready) {
      write_ready_cond.wait(lock, [this] { return write_ready; });
    }
    std::memcpy(input_buffer + offset, buffer, sizeof(T_in) * len);
    buf_size = len + offset;

    write_ready = false;
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
