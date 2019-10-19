#ifndef OUTPUT_H
#define OUTPUT_H

#include <fstream>
#include <string>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "block.h"
#include "task.h"


class output_wrapper : public block {
private:
    std::shared_ptr <block> source;
    std::unique_ptr<task> emit_samples;
    std::thread worker_t;
    std::ofstream file;

    uint32_t buf_size;
    uint16_t buf[MAXIMUM_BUFFER_LENGTH];
    std::mutex buffer_lock;
    std::condition_variable buffer_ready;

public:
    output_wrapper(std::string filename);
    std::string get_type() const { return "output_wrapper"; }
    void from(std::shared_ptr<block> b);
    void run();
    void stop();
    void receive(uint16_t * buffer, uint32_t len);
};

#endif  /* OUTPUT_H */
