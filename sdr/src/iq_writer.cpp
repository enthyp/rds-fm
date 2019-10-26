#include <iostream>
#include "iq_writer.h"


void iq_writer::run()
{
  output -> run();
  input -> run();

  std::cerr << "Running..." << std::endl;
}

void iq_writer::stop()
{
  input -> stop();
  output -> stop();

  std::cerr << "Stopped." << std::endl;
}