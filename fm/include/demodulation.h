#ifndef DEMODULATION_H
#define DEMODULATION_H

#include <string>
#include <memory>
#include "block.h"
#include "task.h"


class demodulator : public block {
private:
  std::shared_ptr <block> sink;
  std::unique_ptr<task> demodulate;
  std::thread worker_t;

  uint32_t buf_size;
  int16_t buf[MAXIMUM_BUFFER_LENGTH];
  std::mutex buffer_lock;
  std::condition_variable buffer_ready;

public:
    std::string get_type() const { return "demodulator"; }
    void to(std::shared_ptr<block> b);
    void run();
    void stop();
    void receive(uint16_t * buffer, uint32_t len);
};

#endif  /* DEMODULATION_H */
