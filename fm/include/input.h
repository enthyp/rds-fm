#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <memory>
#include <thread>

#include "rtl-sdr.h"

#include "block.h"
#include "task.h"


class input_wrapper : public block {
private:
    std::shared_ptr<block> sink;

    rtlsdr_dev_t * dev;
    uint32_t dev_index;

    task emit_samples;
    std::thread worker_t;
    void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx);

    uint32_t buf_len;
    uint16_t buf[MAXIMUM_BUF_LENGTH];

public:
    std::string get_type() const { return "input_wrapper"; }
    void to(std::shared_ptr<block> b);

    void run();
    void stop();
};

#endif  /* INPUT_H */
