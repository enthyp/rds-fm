#include "downsample.h"

int downsampler::run() {
  int j = 0;
  int i = this -> pos;

  while (i < this -> len) {
    this -> sink[j] = this -> source[i];
    i += this -> m_factor;
    j++;
  }

  this -> pos = i % this -> len;
  return j;
}