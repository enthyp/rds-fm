#include <iostream>
#include "receiver.h"


void receiver::run() {
    this -> output -> run();
    this -> demod -> run();
    this -> input -> run();

    std::cout << "Running..." << std::endl;
}

void receiver::stop() {
    this -> output -> stop();
    this -> demod -> stop();
    this -> input -> stop();

    std::cout << "Stopped." << std::endl;
}