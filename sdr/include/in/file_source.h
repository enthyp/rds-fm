#include <fstream>
#include <vector>
#include <atomic>
#include <ring_buffer.h>
#include "basic/block.h"

#ifndef IN_FILE_SOURCE_H
#define IN_FILE_SOURCE_H


class file_source : public producer<int16_t> {
 private:
  std::string source_path;
  std::ifstream source_file;
  int16_t im_buffer[MAXIMUM_BUFFER_LENGTH] {0};

  std::atomic<bool> working;
  void work() override;
  void stop_worker() override { working = false; }

 public:
  explicit file_source(std::string & filepath)
    : source_path {filepath} {};
  std::string get_type() const override { return "file_source"; }
  void run() override
  {
    working = true;
    source_file.open(source_path);
    producer<int16_t>::run();
  }
  void stop() override {
    producer<int16_t>::stop();
    source_file.close();
  }
};

#endif  /* IN_FILE_SOURCE_H */
