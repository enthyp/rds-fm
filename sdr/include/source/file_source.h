#include <fstream>
#include <vector>
#include "base.h"
#include <ring_buffer.h>

#ifndef IN_FILE_SOURCE_H
#define IN_FILE_SOURCE_H


class file_source : public source {
 private:
  std::atomic<bool> working;
  std::string source_path;
  std::ifstream source_file;
  int16_t im_buffer[MAXIMUM_BUFFER_LENGTH] {0};

  void worker() override;
  void stop_worker() override { working = false; }

 public:
  explicit file_source(std::string & filepath)
    : source_path {filepath},
      working {false} {};
  void run() override
  {
    working = true;
    source_file.open(source_path);
    producer<int16_t>::run();
  }
  void stop() override
  {
    producer<int16_t>::stop();
    source_file.close();
  }
};


#endif  /* IN_FILE_SOURCE_H */