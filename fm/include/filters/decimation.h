#ifndef FILTERS_DECIMATION_H
#define FILTERS_DECIMATION_H

class decimator {
 private:
  int16_t * source, * sink;
  int m_factor;

  int init_pos;
  int16_t acc;
  int window_len;
  int window_cnt;

 public:
  downsampler(int16_t * source, int16_t * sink, int m_factor, int window_len)
      : source {source}
  sink {sink}
  m_factor {m_factor}
  init_pos {0}
  acc {0}
  window_len {window_len}
  window_cnt {0} {};

  int run();
};

#endif  /* FILTERS_DECIMATION_H */
