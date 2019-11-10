#include <cmath>
#include "const.h"
#include "flow/fm_demodulation.h"


template <typename T_in, typename T_out>
void fm_demodulator<T_in, T_out>::process_buffer() {
  int len;
  {
    auto lock = this->input_buffer->read_lock();
    int to_read = this->input_buffer->available_read();
    len = demodulate(to_read);
  }

  auto lock = this->output_buffer->write_lock();
  int to_write = this->output_buffer->available_write();
  for (int i = 0; i < std::min(len, to_write); i++) {
    this->output_buffer->push(demodulated_buffer[i]);
  }
}

template <typename T_in, typename T_out>
int fm_demodulator<T_in, T_out>::demodulate(int len) {
  int i;

  for (i = 0; i < len; i += 2) {
    double angle = atan2(this->input_buffer->take(i), this->input_buffer->take(i + 1));
    double angle_diff = angle - prev_angle;

    // Unwrap phase.
    if (angle_diff > PI)
      angle_diff = 2 * PI - angle_diff;
    else if (angle_diff < -PI)
      angle_diff = -2 * PI - angle_diff;

    demodulated_buffer[i / 2] = angle_diff / PI * (1u << 15u);
    prev_angle = angle;
  }
  this->input_buffer->move_read_index(2 * len);
  this->input_buffer->advance_head();

  return int (i / 2);
}


// These are necessary to avoid linkage error.
template class fm_demodulator<int16_t, int16_t>;
template class fm_demodulator<double, int16_t>;
