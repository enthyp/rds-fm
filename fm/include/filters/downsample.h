#ifndef FILTERS_DOWNSAMPLE_H
#define FILTERS_DOWNSAMPLE_H


class downsampler {
 private:
  int len;
  int16_t * source, * sink;
  int m_factor;

  int pos;

 public:
  downsampler(int16_t * source, int len, int16_t * sink, int m_factor)
    : source {source}
      len {len}
      sink {sink}
      m_factor {m_factor}
      pos {0} {};

  int run();
};

#endif  /* FILTERS_DOWNSAMPLE_H */
