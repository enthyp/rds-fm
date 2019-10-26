#ifndef BLOCKS_SINK
#define BLOCKS_SINK

#include <atomic>
#include <cstring>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "block.h"


class sink : public consumer {
 protected:
  int16_t input_buffer[MAXIMUM_BUFFER_LENGTH];
  uint32_t buf_size;
  std::mutex buf_lock;
  std::condition_variable buf_ready;

  virtual void consume() = 0;
  std::thread worker_t;
  virtual void stop_worker() = 0;

 public:
  void run() override = 0;
  void stop() override
  {
    stop_worker();
    worker_t.join();
  }

  void receive(int16_t * buffer, int len) override
  {
    std::lock_guard<std::mutex> lock(buf_lock);
    std::memcpy(input_buffer, buffer, sizeof(int16_t) * len);
    buf_size = len;
    buf_ready.notify_one();
  }
};

#endif  /* BLOCKS_SINK */
