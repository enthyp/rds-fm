#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <memory>
#include <thread>

#include "rtl-sdr.h"

#include "block.h"
#include "task.h"

extern "C" void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx);

class input_wrapper : public block {
private:
    std::shared_ptr<block> sink;

    rtlsdr_dev_t * dev;
    uint32_t dev_index;

    std::unique_ptr<task> emit_samples;
    std::thread worker_t;

    uint32_t buf_len;
    int16_t buf[MAXIMUM_BUFFER_LENGTH];
    friend void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx);

public:
    input_wrapper(uint32_t dev_index);
    std::string get_type() const { return "input_wrapper"; }
    void to(std::shared_ptr<block> b);

    void run();
    void stop();
};

#endif  /* INPUT_H */
