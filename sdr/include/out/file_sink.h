#ifndef OUT_FILE_SINK_H
#define OUT_FILE_SINK_H

#include <fstream>
#include "blocks/sink.h"


class file_sink : public sink {
 private:
  std::ofstream out_file;
  std::ostream * target;
  void consume();
  void stop_worker();
  std::atomic<bool> working;

 public:
  file_sink(std::string filename);
  std::string get_type() const { return "file_sink"; }
  void run();
}

#endif  /* OUT_FILE_SINK_H */
