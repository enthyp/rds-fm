#include <iostream>
#include <unistd.h>
#include "in/file_source.h"


void file_source::produce() {
  int s = 0;
  while (working && source_file.read(reinterpret_cast<char*>(buffer), DEFAULT_BUFFER_LENGTH)) {
    // TODO: use Boost to provide output rate?
    int count = source_file.gcount();
    s += count;
    usleep(100);  // ughhh... TODO: better inter-block data flow!!!
    succ -> receive(buffer, count / sizeof(int16_t));
  }
  std::cout << s << std::endl;
}

void file_source::stop_worker() {
  working = false;
}