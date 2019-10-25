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


class produce_samples : public task {
private:
    rtlsdr_dev_t * dev;
    uint32_t dev_index;
    void * ctx;
    int16_t[MAXIMUM_BUFFER_LENGTH] im_buffer;

public:
    input_producer(rtlsdr_dev_t *dev,
                   uint32_t dev_index,
                   void *ctx)
      : dev {dev},
        dev_index {dev_index},
        ctx {ctx} {};

    void run() {
        rtlsdr_read_async(this -> dev, &rtlsdr_callback, this -> ctx, 0, DEFAULT_BUFFER_LENGTH);
    }

    void stop() {
        task::stop();
        if (rtlsdr_cancel_async(this -> dev) != 0) {
            throw input_shutdown_exception(this -> dev_index);
        }
    }

    // TODO: maybe consumer should pull data from producer?
    // Now producer does not wait for the consumer (might overwrite o.O)
    extern "C" void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx) {
      source * inp = (source*) ctx;
      std::shared_ptr<sink> sink = inp -> sink;

      for (int i = 0; i < (int)len; i++) {
        im_bufer[i] = (int16_t)buf[i] - 127;
      }

      sink -> receive(im_buffer, len);
    }
};

input_wrapper::input_wrapper(uint32_t device_index)
    : dev_index {device_index} {
    // Initialize the device.
    if (rtlsdr_open(&(this -> dev), this -> dev_index) < 0) {
        throw input_init_exception(this -> dev_index);
    }

    rtlsdr_reset_buffer(this -> dev);

    // Set frequency to Chili Zet (93.7 MHz).
    rtlsdr_set_center_freq(this -> dev, 101615000);

    // Set signal sampling rate to 2.4 MHz.
    rtlsdr_set_sample_rate(this -> dev, 2400000);

    // Create the task to produce signal samples.
    this -> produce = std::unique_ptr<task>(
        new produce_samples(this -> dev, this -> dev_index, this));
}

void input_wrapper::stop() {
    // Prompt the producer thread to stop and wait.
    source::stop();

    // Shutdown communication with the device.
    if (rtlsdr_close(this -> dev) < 0) {
        throw input_shutdown_exception(this -> dev_index);
    }
}
