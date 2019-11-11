#include <cmath>
#include "const.h"
#include "flow/fm_demodulation.h"


template <typename T_in, typename T_out>
uint32_t fm_demodulator<T_in, T_out>::process_buffer(int len) {
  int i;

  for (i = 0; i < len; i += 2) {
    double angle = atan2(this->input_buffer->take(i), this->input_buffer->take(i + 1));
    double angle_diff = angle - prev_angle;

    // Unwrap phase.
    if (angle_diff > PI)
      angle_diff = 2 * PI - angle_diff;
    else if (angle_diff < -PI)
      angle_diff = -2 * PI - angle_diff;

    this->intermediate_buffer[i / 2] = angle_diff / PI * (1u << 15u);
    prev_angle = angle;
  }
  this->input_buffer->advance_head(i);

  return int (i / 2);
}


// These are necessary to avoid linkage error.
template class fm_demodulator<int16_t, int16_t>;
template class fm_demodulator<double, int16_t>;
