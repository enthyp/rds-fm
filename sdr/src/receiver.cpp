#include <iostream>
#include "receiver.h"

template <typename T_decimated, typename T_demodulated, typename T_out>
void receiver<T_decimated, T_demodulated, T_out>::run() {
  output -> run();
  downsampler -> run();
  fm_demodulator -> run();
  decimator -> run();
  input -> run();

  std::cerr << "Running..." << std::endl;
}

template <typename T_decimated, typename T_demodulated, typename T_out>
void receiver<T_decimated, T_demodulated, T_out>::stop() {
  input -> stop();
  decimator -> stop();
  fm_demodulator -> stop();
  downsampler -> stop();
  output -> stop();

  std::cerr << "Stopped." << std::endl;
}

// These are necessary to avoid linkage error.
template class receiver<double, int16_t, int16_t>;
template class receiver<double, double, int16_t>;
template class receiver<double, double, double>;
