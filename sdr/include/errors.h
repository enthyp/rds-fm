#ifndef ERRORS_H
#define ERRORS_H

#include <stdexcept>
#include <string>


class block_conn_exception : public std::runtime_error {
 public:
    block_conn_exception(std::string & from, std::string & to) :
        std::runtime_error("Cannot connect " + from + " with " + to) {};
};

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

class bad_call_exception : public std::runtime_error {
  public:
    bad_call_exception(std::string & block_type, std::string & method) :
        std::runtime_error(method + " cannot be called on " + block_type) {};
};


#endif  /* ERRORS_H */
