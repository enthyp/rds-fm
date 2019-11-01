#include <iostream>
#include "test.h"


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
