#ifndef IN_RTL_SOURCE_H
#define IN_RTL_SOURCE_H

#include <string>
#include <memory>
#include "rtl-sdr.h"
#include "basic/source.h"


extern "C" void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx);

class rtl_source : public source {
private:
  rtlsdr_dev_t * dev;
  uint32_t dev_index;
  int16_t im_buffer[MAXIMUM_BUFFER_LENGTH];

  void produce() override ;
  void stop_worker() override ;

  friend void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx);

public:
  explicit rtl_source(uint32_t dev_index);
  std::string get_type() const override { return "rtl_source"; }
  void run() override
  {
    worker_t = std::thread(&rtl_source::produce, this);
  }
  void stop() override;
};

#endif  /* IN_RTL_SOURCE_H */
