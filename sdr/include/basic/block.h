#ifndef BLOCK_H
#define BLOCK_H

#include <memory>
#include <ring_buffer.h>

#define DEFAULT_BUFFER_LENGTH 16384
#define MAXIMUM_BUFFER_LENGTH (16 * DEFAULT_BUFFER_LENGTH)


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
 public:
  ring_buffer<T, MAXIMUM_BUFFER_LENGTH> output_buffer;
};

template <typename T>
class consumer : public block {
 protected:
  std::shared_ptr<producer<T>> pred;

 public:
  virtual void from(std::shared_ptr<consumer<T>> c) override { pred = c; }
};

#endif  /* BLOCK_H */
