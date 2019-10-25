#ifndef BLOCKS_SINK
#define BLOCKS_SINK

#include "block.h"


class sink : public block {
 protected:
  int16_t[MAXIMUM_BUFFER_LENGTH] input_buffer;
  uint32_t buf_size;
  std::mutex & buf_lock;
  std::condition_variable & buf_ready;

  std::unique_ptr<task> consume;
  std::thread worker_t;

 public:
  virtual void run()
  {
    worker_t = std::thread(std::ref(*consume));
  }

  virtual void stop()
  {
    this -> consume -> stop();
    this -> worker_t.join();
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
