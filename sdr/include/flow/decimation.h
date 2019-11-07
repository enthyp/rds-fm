#ifndef BLOCKS_DECIMATION_H
#define BLOCKS_DECIMATION_H

#include <cmath>
#include <atomic>
#include <vector>
#include "flow/decimation.h"


template <class T_in, class T_out>
class decimator : public flow<T_in, T_out> {
 protected:
  int m_factor;
  double fc;
  int kernel_length;
  std::vector<double> kernel;

  int window_cnt;
  T_out decimated_buffer[MAXIMUM_BUFFER_LENGTH];

  void process_buffer() override;
  std::atomic<bool> working;
  virtual int decimate() = 0;

 public:
  decimator(int m_factor, double fc, int kernel_length);
  std::string get_type() const override { return "complex_decimator"; }
};

template <class T_in, class T_out>
class complex_decimator : public decimator<T_in, T_out> {
 private:
  double acc_i;
  double acc_q;
  int decimate(int len) override;

 public:
  complex_decimator(int m_factor, double fc, int kernel_length);
};

template <class T_in, class T_out>
class real_decimator : public decimator<T_in, T_out> {
 private:
  double acc;
  int decimate(int len) override;

 public:
  real_decimator(int m_factor, double fc, int kernel_length);
};

#endif  /* BLOCKS_DECIMATION_H */
