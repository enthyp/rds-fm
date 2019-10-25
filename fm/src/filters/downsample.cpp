#include "downsample.h"

int downsampler::run(int len) {
  int j = 0;
  int i = this -> pos;

  while (i < len) {
    this -> sink[j] = this -> source[i];
    i += this -> m_factor;
    j++;
  }

  this -> pos = i % len;
  return j;
}