#include "test.h"

int main(int argc, char * argv[]) {
  uint32_t dev_index = 0;
  int freq = 101596000, sampling_rate = 2400000;
  int kernel_length1 = 257, kernel_length2 = 257;
  std::string source = "../data/iq_lna";
  std::string target = std::string(argv[1]);

  iq_writer recv(dev_index, freq, sampling_rate, source, target);
  //decim_writer<double> recv(dev_index, freq, sampling_rate, kernel_length1, source, target);
  //demod_writer<double> recv(dev_index, freq, sampling_rate, kernel_length1, target);
  //fm_receiver recv(dev_index, freq, sampling_rate, kernel_length1, kernel_length2, target);
  recv.run();

  getchar();
  recv.stop();

  return 0;
}
