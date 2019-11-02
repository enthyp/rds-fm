#ifndef RECEIVER_H
#define RECEIVER_H

#include <basic/source.h>
#include "basic/flow.h"
#include "basic/sink.h"
#include "iostream"

template <class T_decimated, class T_demodulated, class T_out>
class receiver {
private:
  std::shared_ptr<source> input;
  std::shared_ptr<flow<int16_t, T_decimated>> decimator;
  std::shared_ptr<flow<T_decimated, T_demodulated>> fm_demodulator;
  std::shared_ptr<flow<T_demodulated, T_out>> downsampler;
  std::shared_ptr<sink<T_out>> output;

public:
  receiver(std::shared_ptr<source> & input,
           std::shared_ptr<flow<int16_t, T_decimated>> & decimator,
           std::shared_ptr<flow<T_decimated, T_demodulated>> & fm_demodulator,
           std::shared_ptr<flow<T_demodulated, T_out>> & downsampler,
           std::shared_ptr<sink<T_out>> & output)
           : input {input},
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