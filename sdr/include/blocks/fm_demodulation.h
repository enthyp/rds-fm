#ifndef DEMODULATION_H
#define DEMODULATION_H

#include <string>
#include <memory>
#include "basic/flow.h"


class fm_demodulator : public flow {
 private:
  // TODO: private demodulation params...
  void process();
  void stop_worker();

  double prev_angle;
  int16_t demod_buffer[MAXIMUM_BUFFER_LENGTH];
  int demodulate();

 public:
  fm_demodulator()
    : prev_angle {0} {};

  std::string get_type() const { return "fm_demodulator"; }
  void run();
};

#endif  /* DEMODULATION_H */
