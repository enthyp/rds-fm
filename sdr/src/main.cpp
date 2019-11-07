#include "test.h"

int main(int argc, char * argv[]) {
  uint32_t dev_index = 0;
  int freq = 101596000, sampling_rate = 2400000;
  int kernel_length1 = 257, kernel_length2 = 257;
  std::string target = std::string(argv[1]);

  //iq_writer recv = iq_writer(dev_index, freq, sampling_rate, target);
  //decim_writer<double> recv = decim_writer<double>(dev_index, freq, sampling_rate, kernel_length1, target);
  //demod_writer<double> recv = demod_writer<double>(dev_index, freq, sampling_rate, kernel_length1, target);
  fm_receiver recv = fm_receiver(dev_index, freq, sampling_rate, kernel_length1, kernel_length2, target);
  recv.run();

  getchar();
  recv.stop();

  return 0;
}
