#include <iostream>
#include <memory>
#include <thread>
#include <cstddef>
#include <cstring>

#include "rtl-sdr.h"

#include "block.h"
#include "input.h"
#include "errors.h"
#include "task.h"


extern "C" void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx) {
    input_wrapper * inp = (input_wrapper*) ctx;
    std::shared_ptr<block> sink = inp -> sink;

    for (int i = 0; i < (int)len; i++) {
        inp -> buf[i] = (int16_t)buf[i] - 127;
    }
    sink -> receive(inp -> buf, len);
}


class input_producer : public task {
private:
    rtlsdr_dev_t * dev;
    uint32_t dev_index;
    void * ctx;
    uint32_t & buf_num;

public:
    input_producer(
            rtlsdr_dev_t *dev,
            uint32_t dev_index,
            void *ctx,
            uint32_t buf_num):
        dev {dev}, dev_index {dev_index}, ctx {ctx}, buf_num {buf_num} {};

    void run() {
        rtlsdr_read_async(this -> dev, &rtlsdr_callback, this -> ctx, 0, DEFAULT_BUFFER_LENGTH);
    }

    void stop() {
        task::stop();
        if (rtlsdr_cancel_async(this -> dev) != 0) {
            throw input_shutdown_exception(this -> dev_index);
        }
    }
};

input_wrapper::input_wrapper(uint32_t device_index) :
        dev_index {device_index} {
    // Initialize the device.
    if (rtlsdr_open(&(this -> dev), this -> dev_index) < 0) {
        throw input_init_exception(this -> dev_index);
    }

    rtlsdr_reset_buffer(this -> dev);

    // Create the task to produce signal samples.
    this -> emit_samples = std::unique_ptr<task>(new input_producer(this -> dev, this -> dev_index, this, 0));
}

void input_wrapper::to(std::shared_ptr<block> b) {
    this -> sink = b;
}

void input_wrapper::run() {
    this -> worker_t = std::thread(std::ref(*(this -> emit_samples)));
}

void input_wrapper::stop() {
    // Prompt the producer thread to stop and wait.
    this -> emit_samples -> stop();
    this -> worker_t.join();

    // Shutdown communication with the device.
    if (rtlsdr_close(this -> dev) < 0) {
        throw input_shutdown_exception(this -> dev_index);
    }
}
