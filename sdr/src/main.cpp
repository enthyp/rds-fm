#include <iostream>
#include "test.h"

int main(int argc, char * argv[]) {
  uint32_t dev_index = 0;
  int freq, sampling_rate = 2400000;
  int kernel_length1 = 129, kernel_length2 = 257;
  std::string source = "../data/iq_speech";
  freq = std::stoi(argv[1]);
  std::string target = std::string(argv[2]);

  //iq_writer recv(dev_index, freq, sampling_rate, source, target);
  //decim_writer<double> recv(dev_index, freq, sampling_rate, kernel_length1, source, target);
  //demod_writer<double> recv(dev_index, freq, sampling_rate, kernel_length1, source, target);
  fm_receiver recv(dev_index, freq, sampling_rate, kernel_length1, kernel_length2, source, target);
  recv.run();

  getchar();
  recv.stop();

  return 0;
}
