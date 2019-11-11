#include <cmath>
#include "const.h"
#include "flow/fm_demodulation.h"


template <typename T_in, typename T_out>
void fm_demodulator<T_in, T_out>::process_buffer() {
  uint32_t len;
  std::unique_lock<std::mutex> r_lock(this->input_buffer->m);
  this->input_buffer->read_v.wait(r_lock, [this] { return this->input_buffer->read_c; });
  int to_read = this->input_buffer->available_read();
  len = demodulate(to_read);

  std::unique_lock<std::mutex> w_lock(this->output_buffer->m);
  mem_block<T_out> b = {demodulated_buffer, len};
  this->output_buffer->push_block(b);

  this->output_buffer->read_c = true;
  w_lock.unlock();
  this->output_buffer->read_v.notify_one();

  this->input_buffer->read_c = false;
  r_lock.unlock();
}

template <typename T_in, typename T_out>
uint32_t fm_demodulator<T_in, T_out>::demodulate(int len) {
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
  this->input_buffer->advance_head(i);

  return int (i / 2);
}


// These are necessary to avoid linkage error.
template class fm_demodulator<int16_t, int16_t>;
template class fm_demodulator<double, int16_t>;
