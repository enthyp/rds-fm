#ifndef OUT_FILE_SINK_H
#define OUT_FILE_SINK_H

#include <fstream>
#include "basic/sink.h"


class file_sink : public sink {
 private:
  std::ofstream out_file;
  std::ostream * target;
  void consume() override;
  void stop_worker() override;
  std::atomic<bool> working;

 public:
  explicit file_sink(std::string & filename);
  std::string get_type() const override { return "file_sink"; }
  void run() override
  {
    working = true;
    worker_t = std::thread(&file_sink::consume, this);
  }
};

#endif  /* OUT_FILE_SINK_H */
