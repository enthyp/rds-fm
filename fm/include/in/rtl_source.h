#ifndef IN_RTL_SOURCE_H
#define IN_RTL_SOURCE_H

#include <string>
#include <memory>
#include <thread>

#include "rtl-sdr.h"

#include "../blocks/block.h"
#include "../blocks/task.h"


class input_wrapper : public source {
private:
    rtlsdr_dev_t * dev;
    uint32_t dev_index;
    friend produce_samples;

public:
    input_wrapper(uint32_t dev_index);
    std::string get_type() const { return "input_wrapper"; }
    void to(std::shared_ptr<consumer> c);

    void run();
    void stop();
};

#endif  /* IN_RTL_SOURCE_H */
