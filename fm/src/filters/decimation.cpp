#include "decimation.h"

int decimator:run(int len) {
  int i = 0;
  int j = this -> init_pos;

  while (j < len) {
    while (this -> window_cnt < this -> window_len && j < len) {
      this -> acc += this -> source[j];
      this -> window_cnt++;
      j++;
    }

    if (this -> window_cnt == this -> window_len) {
      this -> sink[i] = this -> acc;
      this -> acc = 0;
      this -> window_cnt = 0;
      i++;
    }

    j += (this -> m_factor - this -> window_len);
  }

  this -> init_pos = j % len;
  return i;
};