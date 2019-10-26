#ifndef BLOCKS_SINK
#define BLOCKS_SINK

#include <atomic>
#include "block.h"


class sink : public consumer {
 protected:
  int16_t[MAXIMUM_BUFFER_LENGTH] input_buffer;
  uint32_t buf_size;
  std::mutex & buf_lock;
  std::condition_variable & buf_ready;

  virtual void consume() = 0;
  std::thread worker_t;
  virtual void stop_worker() = 0;

 public:
  virtual void run()
  {
    worker_t = std::thread(consume);
  }

  virtual void stop()
  {
    stop_worker();
    worker_t.join();
  }

  virtual void receive(int16_t * buffer, uint32_t len)
  {
    std::lock_guard<std::mutex> lock(buf_lock);
    std::memcpy(buffer, input_buffer, sizeof(int16_t) * len);
    buf_size = len;
    buf_ready.notify_one();
  }
};

#endif  /* BLOCKS_SINK */
