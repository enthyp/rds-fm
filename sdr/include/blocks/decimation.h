#ifndef BLOCKS_DECIMATION_H
#define BLOCKS_DECIMATION_H

#include <cmath>
#include <atomic>
#include <vector>
#include "basic/flow.h"


class complex_decimator : public flow {
 private:
  int m_factor;
  double fc;
  int kernel_length;
  std::vector<double> kernel;

  int window_cnt;
  double acc_i;
  double acc_q;
  int16_t decimated_buffer[MAXIMUM_BUFFER_LENGTH];

  void process() override;
  void stop_worker() override;
  std::atomic<bool> working;
  int decimate();

 public:
  complex_decimator(int m_factor, double fc, int kernel_length);
  std::string get_type() const override { return "complex_decimator"; }
  void run() override
  {
    working = true;
    worker_t = std::thread(&complex_decimator::process, this);
  }
};

#endif  /* BLOCKS_DECIMATION_H */
