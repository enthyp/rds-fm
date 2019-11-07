#ifndef DEMODULATION_H
#define DEMODULATION_H

#include <cmath>
#include <string>
#include <memory>
#include <atomic>
#include "base.h"


template <class T_in, class T_out>
class fm_demodulator : public flow<T_in, T_out> {
 private:
  void process_buffer() override;

  double prev_angle;
  T_out demodulated_buffer[MAXIMUM_BUFFER_LENGTH];
  int demodulate(int len);

 public:
  fm_demodulator()
    : flow<T_in, T_out>(),
      demodulated_buffer {0},
      consumer<T_in>::working {false},
      prev_angle {0} {};
  std::string get_type() const override { return "fm_demodulator"; }
};

#endif  /* DEMODULATION_H */
