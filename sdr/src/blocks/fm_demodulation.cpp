#include "const.h"
#include "blocks/fm_demodulation.h"


template <typename T_in, typename T_out>
void fm_demodulator<T_in, T_out>::process() {
  while (working) {
    // Wait for data in the buffer.
    std::unique_lock<std::mutex> lock(flow<T_in, T_out>::buf_lock);
    if (!flow<T_in, T_out>::read_ready) {
      flow<T_in, T_out>::read_ready_cond.wait(lock);
    }

    if (!working)
      break;

    int len = demodulate();

    // And send the data to output block.
    flow<T_in, T_out>::succ -> receive(demodulated_buffer, len);

    flow<T_in, T_out>::read_ready = false;
    lock.unlock();
  }
}

template <typename T_in, typename T_out>
int fm_demodulator<T_in, T_out>::demodulate() {
  int i;

  for (i = 0; i < flow<T_in, T_out>::buf_size; i += 2) {
    double angle = atan2(flow<T_in, T_out>::input_buffer[i], flow<T_in, T_out>::input_buffer[i + 1]);
    double angle_diff = angle - prev_angle;

    // Unwrap phase.
    if (angle_diff > PI)
      angle_diff = 2 * PI - angle_diff;
    else if (angle_diff < -PI)
      angle_diff = -2 * PI - angle_diff;

    demodulated_buffer[i / 2] = convert(angle_diff);
    prev_angle = angle;
  }

  return int (i / 2);
}

template <typename T_in, typename T_out>
void fm_demodulator<T_in, T_out>::stop_worker() {
  working = false;
  this -> read_ready = true;
  flow<T_in, T_out>::read_ready_cond.notify_one();
}


template <>
double convert_angle<double>::operator()(double angle) {
  return angle;  // / 3.14159 * (1u<<15u);
}

template <>
int16_t convert_angle<int16_t>::operator()(double angle) {
  return (int16_t)(angle / 3.14159 * (1u<<15u));
}

// These are necessary to avoid linkage error.
template class fm_demodulator<double, double>;
template class fm_demodulator<double, int16_t>;
