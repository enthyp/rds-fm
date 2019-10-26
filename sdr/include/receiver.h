#ifndef RECEIVER_H
#define RECEIVER_H

#include "blocks/block.h"


class receiver {
private:
  std::shared_ptr<source> input;
  std::shared_ptr<flow> decimator;
  std::shared_ptr<flow> fm_demodulator;
  std::shared_ptr<flow> downsampler;
  std::shared_ptr<sink> output;

public:
  receiver(std::shared_ptr<source> input,
           std::shared_ptr<flow> decimator,
           std::shared_ptr<flow> fm_demodulator,
           std::shared_ptr<flow> downsampler,
           std::shared_ptr<sink> output)
           : input {i},
             decimator {decimator},
             fm_demodulator {fm_demodulator},
             downsampler {downsampler},
             output {output} {
        input -> to(decimator);
        decimator -> to(fm_demodulator);
        fm_demodulator -> to(downsampler);
        downsampler -> to(output);
    };

    void run();
    void stop();
};

#endif  /* RECEIVER_H */