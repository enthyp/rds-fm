#include <iostream>
#include "decim_writer.h"


void decim_writer::run()
{
  output -> run();
  decimator -> run();
  input -> run();

  std::cerr << "Running..." << std::endl;
}

void decim_writer::stop()
{
  input -> stop();
  decimator -> stop();
  output -> stop();

  std::cerr << "Stopped." << std::endl;
}