#ifndef BLOCKS_DECIMATION_H
#define BLOCKS_DECIMATION_H

#include <cmath>
#include <atomic>
#include <vector>
#include "basic/flow.h"


template <class T_in, class T_out>
class decimator : public flow<T_in, T_out> {
 protected:
  int m_factor;
  double fc;
  int kernel_length;
  std::vector<double> kernel;

  int window_cnt;
  T_out decimated_buffer[MAXIMUM_BUFFER_LENGTH];

  void process() override;
  void stop_worker() override;
  std::atomic<bool> working;
  virtual int decimate() = 0;

 public:
  decimator(int m_factor, double fc, int kernel_length);
  std::string get_type() const override { return "complex_decimator"; }
  void run() override
  {
    working = true;
    flow<T_in, T_out>::worker_t = std::thread(&decimator::process, this);
  }
};

template <class T_in, class T_out>
class complex_decimator : public decimator<T_in, T_out> {
 private:
  double acc_i;
  double acc_q;
  int decimate() override;

 public:
  complex_decimator(int m_factor, double fc, int kernel_length);
};

template <class T_in, class T_out>
 private:
  double acc;
  int decimate() override;

 public:
  real_decimator(int m_factor, double fc, int kernel_length);
};

#endif  /* BLOCKS_DECIMATION_H */
