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
  double prev_angle;
  T_out demodulated_buffer[MAXIMUM_BUFFER_LENGTH];

  void process_buffer() override;
  int demodulate(int len);

 public:
  fm_demodulator()
    : flow<T_in, T_out>(),
      demodulated_buffer {0},
      prev_angle {0} {};
};

#endif  /* DEMODULATION_H */
