#ifndef BLOCKS_DECIMATION_H
#define BLOCKS_DECIMATION_H

#include <atomic>
#include "basic/flow.h"


class complex_decimator : public flow {
 private:
  int m_factor;
  int window_len;

  int init_pos;
  int32_t acc_i;
  int32_t acc_q;
  int window_cnt;
  int16_t decimated_buffer[MAXIMUM_BUFFER_LENGTH];

  void process() override;
  void stop_worker() override;
  std::atomic<bool> working;
  int decimate();

 public:
  complex_decimator(int m_factor, int window_len)
    : m_factor {m_factor},
      window_len {window_len},
      decimated_buffer {0},
      working {false},
      init_pos {0},
      acc_i {0},
      acc_q {0},
      window_cnt {0} {};
  std::string get_type() const override { return "complex_decimator"; }
  void run() override
  {
    working = true;
    worker_t = std::thread(&complex_decimator::process, this);
  }
};

#endif  /* BLOCKS_DECIMATION_H */
