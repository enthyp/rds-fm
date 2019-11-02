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
  T input_buffer[MAXIMUM_BUFFER_LENGTH];
  uint32_t buf_size;
  std::mutex buf_lock;
  std::condition_variable read_ready_cond;
  bool read_ready;
  std::condition_variable write_ready_cond;
  bool write_ready;

  virtual void consume() = 0;
  std::thread worker_t;
  virtual void stop_worker() = 0;

 public:
  sink() : read_ready {false}, write_ready {true} {};
  void run() override = 0;
  void stop() override
  {
    stop_worker();
    worker_t.join();
  }

  void receive(T * buffer, int len) override
  {
    std::unique_lock<std::mutex> lock(buf_lock);
    if (!write_ready) {
      write_ready_cond.wait(lock, [this] { return write_ready; });
    }

    std::memcpy(input_buffer, buffer, sizeof(T) * len);
    buf_size = len;

    write_ready = false;
    read_ready = true;
    lock.unlock();
    read_ready_cond.notify_one();
  }
};

#endif  /* BLOCKS_SINK */
