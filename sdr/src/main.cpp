#include <iostream>
#include <memory>

#include "in/rtl_source.h"
#include "blocks/decimation.h"
#include "blocks/fm_demodulation.h"
#include "blocks/downsampling.h"
#include "out/file_sink.h"
#include "receiver.h"


int main(int argc, char* argv[]) {
  int dev_index = 0;
  int freq = 101615000, sampling_rate = 2400000;
  int m1 = 10, m2 = 5;
  std::string target = std::string(argv[1]);

  std::shared_ptr<source> input = std::shared_ptr<source>(
      new rtl_source(dev_index, freq, sampling_rate));
  std::shared_ptr<flow> decimator = std::shared_ptr<flow>(
      new complex_decimator(m1, m1));
  std::shared_ptr<flow> fm_demodulator = std::shared_ptr<flow>(
      new class::fm_demodulator());
  std::shared_ptr<flow> downsampler = std::shared_ptr<flow>(
      new class::downsampler(m2));
  std::shared_ptr<sink> output = std::shared_ptr<sink>(
      new file_sink(target));

  receiver recv = receiver(input, decimator, fm_demodulator, downsampler, output);
  recv.run();

  std::this_thread::sleep_for(std::chrono::milliseconds(10000));
  recv.stop();

  return 0;
}
