#ifndef RECEIVER_H
#define RECEIVER_H

#include "input.h"
#include "demodulation.h"
#include "output.h"


class receiver {
private:
    std::shared_ptr<input_wrapper> input;
    std::shared_ptr<demodulator> demod;
    std::shared_ptr<output_wrapper> output;

public:
    receiver(std::shared_ptr<input_wrapper> i, std::shared_ptr<demodulator> d, std::shared_ptr<output_wrapper> o) :
            input {i}, demod {d}, output {o} {
        this -> input -> to(output);
    };

    void run();

    void stop();
};

#endif  /* RECEIVER_H */