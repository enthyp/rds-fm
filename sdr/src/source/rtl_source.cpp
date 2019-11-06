#include <iostream>
#include <memory>
#include <thread>
#include <cstddef>
#include <cstring>

#include "rtl-sdr.h"

#include "in/rtl_source.h"
#include "errors.h"


rtl_source::rtl_source(uint32_t device_index, int freq, int sampling_rate)
    : dev_index {device_index} {
    // Initialize the device.
    if (rtlsdr_open(&(dev), dev_index) < 0) {
        throw input_init_exception(dev_index);
    }

    rtlsdr_reset_buffer(dev);

    rtlsdr_set_center_freq(dev, freq);
    rtlsdr_set_sample_rate(dev, sampling_rate);
    rtlsdr_set_tuner_gain_mode(dev, 0);
}

extern "C" void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx);

void rtl_source::work() {
  rtlsdr_read_async(dev, &rtlsdr_callback, this, 0, DEFAULT_BUFFER_LENGTH);
}

void rtl_source::stop_worker() {
  if (rtlsdr_cancel_async(dev) != 0) {
    throw input_shutdown_exception(dev_index);
  }
}

void rtl_source::stop() {
  // Prompt the producer thread to stop and wait.
  producer<int16_t>::stop();

  // Shutdown communication with the device.
  if (rtlsdr_close(dev) < 0) {
      throw input_shutdown_exception(dev_index);
  }
}


// TODO: maybe consumer should pull data from producer?
// Now producer does not wait for the consumer (might overwrite o.O)
extern "C" void rtlsdr_callback(unsigned char * buf, uint32_t len, void *ctx) {
  auto source = (rtl_source*) ctx;

  if (len <= source -> output_buffer . available_write()) {
    for (int i = 0; i < (int)len; i++) {
      source -> output_buffer.push((int16_t)buf[i] - 127);
    }
  }  // ...else drop it all.
}
