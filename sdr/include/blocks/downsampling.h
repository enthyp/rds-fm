#ifndef BLOCKS_DOWNSAMPLING_H
#define BLOCKS_DOWNSAMPLING_H

#include <atomic>
#include "basic/flow.h"


class downsampler : public flow {
 private:
  int m_factor;
  int16_t downsampled_buffer[MAXIMUM_BUFFER_LENGTH];
  int pos;

  void process() override;
  std::atomic<bool> working;
  void stop_worker() override;
  int downsample();

 public:
  explicit downsampler(int m_factor)
    : flow(),
      m_factor {m_factor},
      pos {0},
      downsampled_buffer {0},
      working {false} {};
  std::string get_type() const override { return "downsampler"; }
  void run() override
  {
    working = true;
    worker_t = std::thread(&downsampler::process, this);
  }
};

#endif  /* BLOCKS_DOWNSAMPLING_H */
