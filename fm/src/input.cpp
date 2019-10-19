#include <memory>
#include <thread>
#include <cstddef>
#include <cstring>

#include "rtl-sdr.h"

#include "block.h"
#include "input.h"
#include "errors.h"
#include "task.h"


input_wrapper::input_wrapper(int device_index) :
    dev_index {device_index} {

    // Initialize the device.
    if (rtlsdr_open(&(this -> dev), this -> dev_index) < 0) {
        throw input_init_exception(this -> dev_index);
    }

    rtlsdr_reset_buffer(this -> dev);

    // Create the task to produce signal samples.
    this -> emit_samples = class : task {
        void run() {
            rtlsdr_read_async(this -> dev, this -> rtlsdr_callback, this, 0, this -> buf_len);
        }
    };
}

void input_wrapper::to(std::shared_ptr<block> b) {
    this -> sink = b;
}

void input_wrapper::run() {
    this -> worker_t = std::thread(emit_samples);
}

void input_wrapper::stop() {
    // Prompt the producer thread to stop and wait.
    this -> emit_samples -> stop();
    this -> worker_t.join();

    // Shutdown communication with the device.
    if (rtlsdr_cancel_async(this -> dev) < 0 || rtlsdr_close(this -> dev) < 0) {
        throw input_shutdown_exception(this -> dev_index);
    }
}

void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx) {
    input_wrapper * input_wrapper = (input_wrapper*) ctx;
    std::shared_ptr<block> sink = input_wrapper -> sink;
    std::memcpy(input_wrapper -> buf, buf, len);

    // TODO: additional processing before passing over to the next stage.
    sink -> receive(buf, len);
}