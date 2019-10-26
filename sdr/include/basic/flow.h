#ifndef BLOCKS_FLOW_H
#define BLOCKS_FLOW_H

#include "block.h"


class flow : public producer, public consumer {
 protected:
  std::shared_ptr<consumer> consumer;

  int16_t[MAXIMUM_BUFFER_LENGTH] input_buffer;
  uint32_t buf_size;
  std::mutex & buf_lock;
  std::condition_variable & buf_ready;

  virtual void process() = 0;
  std::thread worker_t;
  virtual void stop_worker() = 0;

 public:
  virtual void run()
  {
    worker_t = std::thread(process);
  }


  virtual void stop()
  {
    stop_worker();
    worker_t.join();
  }

  virtual void receive(int16_t * buffer, int len)
  {
    std::lock_guard<std::mutex> lock(buf_lock);
    std::memcpy(buffer, input_buffer, sizeof(int16_t) * len);
    buf_size = len;
    buf_ready.notify_one();
  }

  virtual void to(std::shared_ptr<consumer> c)
  {
    consumer = c;
  }
};


#endif  /* BLOCKS_FLOW_H */
