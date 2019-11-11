#ifndef BLOCK_H
#define BLOCK_H

#include <atomic>
#include <thread>
#include <memory>
#include "const.h"
#include <ring_buffer.h>


class block {
 protected:
  std::thread worker_t;
  virtual void worker() = 0;
  virtual void stop_worker() = 0;

 public:
  virtual void run() { worker_t = std::thread(&block::worker, this); };
  virtual void stop()
  {
    stop_worker();
    worker_t.join();
  };
};


template <typename T>
class producer : virtual public block {
 protected:
  ring_buffer<T> * output_buffer;
 public:
  void to(ring_buffer<T> & buffer) { output_buffer = &buffer; }
};

template <typename T>
class consumer : virtual public block {
 protected:
  ring_buffer<T> * input_buffer;
 public:
  void from(ring_buffer<T> & buffer) { input_buffer = &buffer; }
};

// By convention, RTL-SDR produces int16_t and so shall any other source.
typedef producer<int16_t> source;

template <class T_in, class T_out>
class flow : public producer<T_out>, public consumer<T_in> {
 protected:
  std::atomic_bool working;
  T_out intermediate_buffer[MAXIMUM_BUFFER_LENGTH];

  void worker() override
  {
    while (working) {
      uint32_t len;
      auto r_lock = this->input_buffer->read_lock();
      int to_read = this->input_buffer->available_read();
      len = process_buffer(to_read);

      {
        auto w_lock = this->output_buffer->write_lock();
        mem_block<T_out> b = {intermediate_buffer, len};
        this->output_buffer->push_block(b);
      }
    }
  };
  void stop_worker() override { working = false; }
  virtual uint32_t process_buffer(int len) = 0;

 public:
  flow<T_in, T_out>() : working {false} {};
  void run() override
  {
    working = true;
    consumer<T_in>::run();
  }
};


#endif  /* BLOCK_H */