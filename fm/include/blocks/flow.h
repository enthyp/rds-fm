#ifndef BLOCKS_FLOW
#define BLOCKS_FLOW

#include "block.h"
#include "sink.h"


class flow : public producer, public consumer {
 protected:
  std::shared_ptr<consumer> consumer;

  int16_t[MAXIMUM_BUFFER_LENGTH] input_buffer;
  uint32_t buf_size;
  std::mutex & buf_lock;
  std::condition_variable & buf_ready;

  std::unique_ptr<task> process;
  std::thread worker_t;

 public:
  virtual void run()
  {
    worker_t = std::thread(std::ref(*process));
  }

  virtual void stop()
  {
    this -> process -> stop();
    this -> worker_t.join();
  }

  virtual void receive(int16_t * buffer, uint32_t len)
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


#endif  /* BLOCKS_FLOW */
