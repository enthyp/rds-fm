#include <iostream>
#include "test.h"

void fm_receiver::run()
{
  output.run();
  decimator2.run();
  demodulator.run();
  decimator.run();
  input.run();

  std::cerr << "Running..." << std::endl;
}

void fm_receiver::stop()
{
  input.stop();
  decimator.stop();
  demodulator.stop();
  decimator2.stop();
  output.stop();

  std::cerr << "Stopped." << std::endl;
}

template <class T>
void demod_writer<T>::run()
{
  output.run();
  demodulator.run();
  decimator.run();
  input.run();

  std::cerr << "Running..." << std::endl;
}

template <class T>
void demod_writer<T>::stop()
{
  input.stop();
  decimator.stop();
  demodulator.stop();
  output.stop();

  std::cerr << "Stopped." << std::endl;
}


template <class T_out>
void decim_writer<T_out>::run()
{
    output.run();
    decimator.run();
    input.run();

    std::cerr << "Running..." << std::endl;
}

template <class T_out>
void decim_writer<T_out>::stop()
{
    input.stop();
    decimator.stop();
    output.stop();

    std::cerr << "Stopped." << std::endl;
}

void iq_writer::run()
{
    output.run();
    input.run();

    std::cerr << "Running..." << std::endl;
}

void iq_writer::stop()
{
    input.stop();
    output.stop();

    std::cerr << "Stopped." << std::endl;
}

// These are necessary to avoid linkage error.
template class demod_writer<double>;
template class demod_writer<int16_t>;
template class decim_writer<double>;
template class decim_writer<int16_t>;