#include <unistd.h>
#include "source/file_source.h"


void file_source::worker() {
  while (working && source_file.read(reinterpret_cast<char *>(im_buffer), DEFAULT_BUFFER_LENGTH)) {
    if (output_buffer -> available_write() >= DEFAULT_BUFFER_LENGTH) {
      int count = source_file.gcount();

      for (int i = 0; i < count; i++) {
        output_buffer -> push(im_buffer[i]);
      }
    }  // else drop it...
    usleep(10);  // ughhh...
  }
}
