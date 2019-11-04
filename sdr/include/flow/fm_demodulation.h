#ifndef DEMODULATION_H
#define DEMODULATION_H

#include <cmath>
#include <string>
#include <memory>
#include <atomic>
#include "basic/flow.h"

template <class T_out>
struct convert_angle{
  T_out operator()(double angle);
};

template <class T_in, class T_out>
class fm_demodulator : public flow<T_in, T_out> {
 private:
  // TODO: private demodulation params...
  void process() override;
  std::atomic<bool> working;
  void stop_worker() override;

  double prev_angle;
  T_out demodulated_buffer[MAXIMUM_BUFFER_LENGTH];
  int demodulate();
  convert_angle<T_out> convert;

 public:
  fm_demodulator()
    : flow<T_in, T_out>(),
      demodulated_buffer {0},
      working {false},
      prev_angle {0} {};

  std::string get_type() const override { return "fm_demodulator"; }
  void run() override
  {
    working = true;
    flow<T_in, T_out>::worker_t = std::thread(&fm_demodulator::process, this);
  }};

#endif  /* DEMODULATION_H */
