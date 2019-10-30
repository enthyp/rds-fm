#include "in/file_source.h"

void file_source::produce() {
  while (working) {
    // TODO: output data from at given rate...
    succ->receive(some_buffer, some_len);
  }
}

void file_source::stop_worker() {
  working = false;
  // TODO: anything else?
}