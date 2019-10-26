#ifndef BLOCKS_DOWNSAMPLING_H
#define BLOCKS_DOWNSAMPLING_H

#include "basic/flow.h"


class downsampler : public flow {
private:
  int m_factor;
  int16_t[MAXIMUM_BUFFER_LENGTH] downsampled_buffer;
  int pos;

  void process();
  void stop_worker();
  int downsample();

public:
  downsampler(int m_factor)
    : m_factor {m_factor}
      pos {0} {};
};

#endif  /* BLOCKS_DOWNSAMPLING_H */
