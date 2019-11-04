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
  virtual void work() = 0;
  virtual void stop_worker() = 0;

 public:
  virtual std::string get_type() const = 0;
  virtual void run() { worker_t = std::thread(&block::work, this); };
  virtual void stop()
  {
    stop_worker();
    worker_t.join();
  };
};


template <typename T>
class producer : public block {
 protected:
  ring_buffer<T, MAXIMUM_BUFFER_LENGTH> * output_buffer;
 public:
  void to(ring_buffer<T, MAXIMUM_BUFFER_LENGTH> & buffer) { output_buffer = buffer; }
};

template <typename T>
class consumer : public block {
 protected:
  ring_buffer<T, MAXIMUM_BUFFER_LENGTH> * input_buffer;
 public:
  void from(ring_buffer<T, MAXIMUM_BUFFER_LENGTH> & buffer) { input_buffer = buffer; }
};

// By convention, RTL-SDR produces int16_t and so shall any other source.
typedef producer<int16_t> source;

template <class T_in, class T_out>
class flow : public producer<T_out>, public consumer<T_in> {
 protected:
  std::atomic_bool working;

  void work() override {
    while (working) {
      // TODO: Get data from input buffer, process them, dump them to the output buffer.

    }
  };
};


#endif  /* BLOCK_H */
