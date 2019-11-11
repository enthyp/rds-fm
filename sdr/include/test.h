#ifndef TEST_H
#define TEST_H

#include <base.h>
#include <source/rtl_source.h>
#include <sink/file_sink.h>
#include <flow/decimation.h>
#include <flow/fm_demodulation.h>
#include <source/file_source.h>

class fm_receiver {
 private:
  //rtl_source input;
  file_source input;
  ring_buffer<int16_t> med1;
  complex_decimator<int16_t, double> decimator;
  ring_buffer<double> med2;
  fm_demodulator<double, int16_t> demodulator;
  ring_buffer<int16_t> med3;
  real_decimator<int16_t , int16_t> decimator2;
  ring_buffer<int16_t> med4;
  file_sink<int16_t> output;

 public:
  fm_receiver(uint32_t dev_index, int freq, int sampling_rate, int kernel_len1, int kernel_len2, std::string & source, std::string & target)
      : input {source},
        //input {dev_index, freq, sampling_rate},
        decimator {10, 1. / (2 * 1.2 * 10), kernel_len1},
        decimator2 {5, 1. / (2 * 1.2 * 5), kernel_len2},
        output {target}
  {
    input.to(med1);
    decimator.from(med1);
    decimator.to(med2);
    demodulator.from(med2);
    demodulator.to(med3);
    decimator2.from(med3);
    decimator2.to(med4);
    output.from(med4);
  };

  void run();
  void stop();
};

template <class T>
class demod_writer {
 private:
  //rtl_source input;
  file_source input;
  ring_buffer<int16_t> med1;
  complex_decimator<int16_t, T> decimator;
  ring_buffer<T> med2;
  fm_demodulator<T, int16_t> demodulator;
  ring_buffer<int16_t> med3;
  file_sink<int16_t> output;

 public:
  demod_writer(uint32_t dev_index, int freq, int sampling_rate, int kernel_len1, std::string & source, std::string & target)
    : //input {dev_index, freq, sampling_rate},
      input (source),
      decimator {10, 1. / (2 * 1.2 * 10), kernel_len1},
      output {target}
  {
    input.to(med1);
    decimator.from(med1);
    decimator.to(med2);
    demodulator.from(med2);
    demodulator.to(med3);
    output.from(med3);
  };

  void run();
  void stop();
};


template <class T>
class decim_writer {
 private:
  //rtl_source input;
  file_source input;
  ring_buffer<int16_t> med1;
  complex_decimator<int16_t, T> decimator;
  ring_buffer<T> med2;
  file_sink<T> output;

public:
    decim_writer(uint32_t dev_index, int freq, int sampling_rate, int kernel_len1, std::string &source, std::string & target)
      : //input {dev_index, freq, sampling_rate},
        input {source},
        decimator {10, 1. / (2 * 1.2 * 10), kernel_len1},
        output {target}
    {
      input.to(med1);
      decimator.from(med1);
      decimator.to(med2);
      output.from(med2);
    };

    void run();
    void stop();
};


class iq_writer {
 private:
  //rtl_source input;
  file_source input;
  ring_buffer<int16_t> med;
  file_sink<int16_t> output;

  public:
    iq_writer(uint32_t dev_index, int freq, int sampling_rate, std::string & source, std::string & target)
      : input {source},
        // input {dev_index, freq, sampling_rate},
        output {target}
    {
      input.to(med);
      output.from(med);
    };

    void run();
    void stop();
};

#endif //TEST_H
