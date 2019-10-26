#include <iostream>
#include <memory>
#include <thread>
#include <cstddef>
#include <cstring>

#include "rtl-sdr.h"

#include "in/rtl_source.h"
#include "errors.h"


rtl_source::rtl_source(uint32_t device_index)
    : dev_index {device_index} {
    // Initialize the device.
    if (rtlsdr_open(&(dev), dev_index) < 0) {
        throw input_init_exception(dev_index);
    }

    rtlsdr_reset_buffer(dev);

    // Set frequency to Chili Zet (93.7 MHz).
    rtlsdr_set_center_freq(dev, 101615000);

    // Set signal sampling rate to 2.4 MHz.
    rtlsdr_set_sample_rate(dev, 2400000);
}

void rtl_source::produce() {
  rtlsdr_read_async(dev, &rtlsdr_callback, NULL, 0, DEFAULT_BUFFER_LENGTH);
}

// TODO: maybe consumer should pull data from producer?
// Now producer does not wait for the consumer (might overwrite o.O)
extern "C" void rtl_source::rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx) {
  for (int i = 0; i < (int)len; i++) {
    im_bufer[i] = (int16_t)buf[i] - 127;
  }

  sink -> receive(im_buffer, len);
}

void rtl_source::stop_worker() {
  if (rtlsdr_cancel_async(dev) != 0) {
    throw input_shutdown_exception(dev_index);
  }
}

void rtl_source::stop() {
  // Prompt the producer thread to stop and wait.
  source::stop();

  // Shutdown communication with the device.
  if (rtlsdr_close(dev) < 0) {
      throw input_shutdown_exception(dev_index);
  }
}
