#include <memory>

#include "in/rtl_source.h"
#include "in/file_source.h"
#include "blocks/decimation.h"
#include "blocks/fm_demodulation.h"
#include "blocks/downsampling.h"
#include "out/file_sink.h"
#include "receiver.h"
#include "test.h"


int main(int argc, char * argv[]) {
  int dev_index = 0;
  int freq = 101600000, sampling_rate = 2400000;
  int m1 = 10, kernel_length = 257;
  double fc1 = 1. / (2 * m1);
  int m2 = 5, kernel_length2 = 257;
  double fc2 = 1. / 12;
  std::string target = std::string(argv[1]);

  // Add input block.
//  std::shared_ptr<source> input = std::shared_ptr<source>(
//      new rtl_source(dev_index, freq, sampling_rate));

  std::string source_file = "../audio/iq_samples";
  std::shared_ptr<source> input = std::shared_ptr<source>(new file_source(source_file));
  // Add processing blocks for demodulation.
  std::shared_ptr<flow> decimator = std::shared_ptr<flow>(
      new complex_decimator(m1, fc1, kernel_length));
  std::shared_ptr<flow> fm_demodulator = std::shared_ptr<flow>(
      new class::fm_demodulator());
//  std::shared_ptr<flow> downsampler = std::shared_ptr<flow>(
//      new class::downsampler(m2));
  std::shared_ptr<flow> decimator2 = std::shared_ptr<flow>(
      new complex_decimator(m2, fc2, kernel_length2));

  // And add a sink block.
  std::shared_ptr<sink> output = std::shared_ptr<sink>(
      new file_sink(target));

  //receiver recv = receiver(input, decimator, fm_demodulator, decimator2, output);
  iq_writer recv = iq_writer(input, output);
  // decim_writer recv = decim_writer(input, decimator, output);
  recv.run();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  recv.stop();

  return 0;
}
