#ifndef TEST_H
#define TEST_H

#include <basic/source.h>
#include "basic/sink.h"
#include <basic/flow.h>


class decim_writer {
private:
    std::shared_ptr<source> input;
    std::shared_ptr<flow> decimator;
    std::shared_ptr<sink> output;

public:
    decim_writer(std::shared_ptr<source> & input,
                 std::shared_ptr<flow> & decimator,
                 std::shared_ptr<sink> & output)
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
    std::shared_ptr<sink> output;

public:
    iq_writer(std::shared_ptr<source> & input,
              std::shared_ptr<sink> & output)
            : input {input},
              output {output}
    {
        input -> to(output);
    };

    void run();
    void stop();
};

#endif //TEST_H
