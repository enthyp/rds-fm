#ifndef ERRORS_H
#define ERRORS_H

#include <stdexcept>
#include <string>


class input_init_exception : public std::runtime_error {
  public:
    explicit input_init_exception(int index) :
        std::runtime_error("Failed to open device " + std::to_string(index)) {};
};

class input_shutdown_exception : public std::runtime_error {
  public:
    explicit input_shutdown_exception(int index) :
        std::runtime_error("Failed to close device " + std::to_string(index)) {};
};


#endif  /* ERRORS_H */
