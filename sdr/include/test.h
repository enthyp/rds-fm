#ifndef TEST_H
#define TEST_H

#include <basic/source.h>
#include "basic/sink.h"
#include <basic/flow.h>

template <class T_decimated, class T_out>
class demod_writer {
 private:
  std::shared_ptr<source> input;
  std::shared_ptr<flow<int16_t, T_decimated>> decimator;
  std::shared_ptr<flow<T_decimated, T_out>> demodulator;
  std::shared_ptr<sink<T_out>> output;

 public:
  demod_writer(std::shared_ptr<source> & input,
               std::shared_ptr<flow<int16_t, T_decimated>> & decimator,
               std::shared_ptr<flow<T_decimated, T_out>> demodulator,
               std::shared_ptr<sink<T_out>> & output)
      : input {input},
        decimator {decimator},
        demodulator {demodulator},
        output {output}
  {
    input -> to(decimator);
    decimator -> to(demodulator);
    demodulator -> to(output);
  };

  void run();
  void stop();
};


template <class T_out>
class decim_writer {
private:
    std::shared_ptr<source> input;
    std::shared_ptr<flow<int16_t, T_out>> decimator;
    std::shared_ptr<sink<T_out>> output;

public:
    decim_writer(std::shared_ptr<source> & input,
                 std::shared_ptr<flow<int16_t, T_out>> & decimator,
                 std::shared_ptr<sink<T_out>> & output)
            : input {input},
              decimator {decimator},
              output {output}
    {
        input -> to(decimator);
        decimator -> to(output);
    };

    void run();
    void stop();
};


class iq_writer {
private:
    std::shared_ptr<source> input;
    std::shared_ptr<sink<int16_t >> output;

public:
    iq_writer(std::shared_ptr<source> & input,
              std::shared_ptr<sink<int16_t>> & output)
            : input {input},
              output {output}
    {
        input -> to(output);
    };

    void run();
    void stop();
};

#endif //TEST_H
