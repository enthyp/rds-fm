#include <memory>

#include "in/rtl_source.h"
#include "in/file_source.h"
#include "flow/decimation.h"
#include "flow/fm_demodulation.h"
#include "out/file_sink.h"
#include "pipeline.h"
#include "test.h"


int main(int argc, char * argv[]) {
  int dev_index = 0;
  int freq = 101596000, sampling_rate = 2400000;
  int m1 = 10, kernel_length = 257;
  double fc1 = 1. / (2 * 1.2 * m1);
  int m2 = 5, kernel_length2 = 257;
  double fc2 = 1. / (2 * 1.2 * m2);
  std::string target = std::string(argv[1]);

  // Add input block.

  //std::string source_file = "../data/iq_speech";
  //std::shared_ptr<source> input = std::shared_ptr<source>(new file_source(source_file));

    std::shared_ptr<source> input = std::shared_ptr<source>(
      new rtl_source(dev_index, freq, sampling_rate));

  // Add processing flow for demodulation.
  std::shared_ptr<flow<int16_t, double>> decimator = std::shared_ptr<flow<int16_t, double>>(
      new complex_decimator<int16_t, double>(m1, fc1, kernel_length));

  std::shared_ptr<flow<double, int16_t>> fm_demodulator = std::shared_ptr<flow<double, int16_t>>(
      new class::fm_demodulator<double, int16_t>());

  std::shared_ptr<flow<int16_t, int16_t>> decimator2 = std::shared_ptr<flow<int16_t, int16_t>>(
      new real_decimator<int16_t, int16_t>(m2, fc2, kernel_length2));

  // And add a sink block.
  std::shared_ptr<sink<int16_t>> output = std::shared_ptr<sink<int16_t>>(
      new file_sink<int16_t>(target));

  pipeline<double, int16_t, int16_t> recv =
      pipeline<double, int16_t , int16_t>(input, decimator, fm_demodulator, decimator2, output);
  //iq_writer recv = iq_writer(input, output);
  //demod_writer<double, int16_t > recv = demod_writer<double, int16_t >(input, decimator, fm_demodulator, output);
  //decim_writer<double> recv = decim_writer<double>(input, decimator, output);
  recv.run();

  getchar();
  recv.stop();

  return 0;
}
