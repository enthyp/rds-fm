#ifndef IN_RTL_SOURCE_H
#define IN_RTL_SOURCE_H

#include <string>
#include <memory>
#include "rtl-sdr.h"
#include "blocks/block.h"


class rtl_source : public source {
private:
  rtlsdr_dev_t * dev;
  uint32_t dev_index;
  int16_t[MAXIMUM_BUFFER_LENGTH] im_buffer;

  void produce();
  void stop_worker();
  extern "C" void rtl_source::rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx) {

public:
  rtl_source(uint32_t dev_index);
  std::string get_type() const { return "rtl_source"; }
  void to(std::shared_ptr<consumer> c);

  void run();
  void stop();
};

#endif  /* IN_RTL_SOURCE_H */
