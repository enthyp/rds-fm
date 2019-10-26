#ifndef BLOCKS_DECIMATION_H
#define BLOCKS_DECIMATION_H

#include "basic/flow.h"


class complex_decimator : public flow {
private:
  int m_factor;
  int window_len;

  int init_pos;
  int32_t acc_i;
  int32_t acc_q;
  int window_cnt;
  int16_t[MAXIMUM_BUFFER_LENGTH] decimated_buffer;

  void process();
  void stop_worker();
  int decimate();

 public:
  complex_decimator(int m_factor, int window_len)
    : m_factor {m_factor},
      window_len {window_len},
      init_pos {0},
      acc {0},
      window_cnt {0} {};
};

#endif  /* BLOCKS_DECIMATION_H */
