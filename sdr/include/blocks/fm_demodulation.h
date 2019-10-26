#ifndef DEMODULATION_H
#define DEMODULATION_H

#include <string>
#include <memory>
#include <atomic>
#include "basic/flow.h"


class fm_demodulator : public flow {
 private:
  // TODO: private demodulation params...
  void process() override;
  std::atomic<bool> working;
  void stop_worker() override;

  double prev_angle;
  int16_t demodulated_buffer[MAXIMUM_BUFFER_LENGTH];
  int demodulate();

 public:
  fm_demodulator()
    : demodulated_buffer {0},
      working {false},
      prev_angle {0} {};

  std::string get_type() const override { return "fm_demodulator"; }
  void run() override
  {
    working = true;
    worker_t = std::thread(&fm_demodulator::process, this);
  }};

#endif  /* DEMODULATION_H */
