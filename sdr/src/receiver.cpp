#include <iostream>
#include "receiver.h"


void receiver::run() {
  output -> run();
//  downsampler -> run();
//  fm_demodulator -> run();
  decimator -> run();
  input -> run();

  std::cerr << "Running..." << std::endl;
}

void receiver::stop() {
  input -> stop();
  decimator -> stop();
//  fm_demodulator -> stop();
//  downsampler -> stop();
  output -> stop();

  std::cerr << "Stopped." << std::endl;
}