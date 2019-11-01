#include <fstream>
#include <vector>
#include <atomic>
#include "basic/source.h"

#ifndef IN_FILE_SOURCE_H
#define IN_FILE_SOURCE_H

class file_source : public source {
 private:
  std::string source_path;
  std::ifstream source_file;
  int16_t buffer[DEFAULT_BUFFER_LENGTH];

  std::atomic<bool> working;

  void produce() override;
  void stop_worker() override;

 public:
  explicit file_source(std::string & filepath)
    : source_path {filepath},
      working {false} {};
  std::string get_type() const override { return "file_source"; }
  void run() override
  {
    source_file.open(source_path);
    working = true;
    worker_t = std::thread(&file_source::produce, this);
  }
  void stop() override {
    source::stop();
    source_file.close();
  };
};

#endif  /* IN_FILE_SOURCE_H */
