#include "const.h"
#include "blocks/fm_demodulation.h"


void fm_demodulator::process() {
  while (working) {
    // Wait for data in the buffer.
    std::unique_lock<std::mutex> lock(buf_lock);
    buf_ready.wait(lock);

    if (!working)
      break;

    int len = demodulate();

    // And send the data to output block.
    succ -> receive(demodulated_buffer, len);
  }
}

int fm_demodulator::demodulate() {
  int i;

  for (i = 0; i < buf_size; i += 2) {
    double angle = atan2(input_buffer[i], input_buffer[i + 1]);
    double angle_diff = angle - prev_angle;

    // Unwrap phase.
    if (angle_diff > PI)
      angle_diff = 2 * PI - angle_diff;
    else if (angle_diff < -PI)
      angle_diff = -2 * PI - angle_diff;

    demodulated_buffer[i / 2] = (int16_t)(angle_diff / 3.14159 * (1u<<15u));
    prev_angle = angle;
  }

  return int (i / 2);
}

void fm_demodulator::stop_worker() {
  working = false;
  buf_ready.notify_one();
}
