#ifndef OUTPUT_H
#define OUTPUT_H

#include <string>
#include <memory>
#include <mutex>
#include "block.h"


class output_wrapper : public block {
private:
    std::shared_ptr <block> source;
    task emit_samples;
    std::thread worker_t;

    uint32_t buf_len;
    uint16_t buf[MAXIMUM_BUF_LENGTH];
    std::mutex buffer_lock;

public:
    std::string get_type() const { return "output_wrapper"; }
    void from(std::shared_ptr<block> b);
    void run();
    void stop();
    void receive(uint16_t * buffer, uint32_t len);
};

#endif  /* OUTPUT_H */
