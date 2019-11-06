#ifndef OUT_FILE_SINK_H
#define OUT_FILE_SINK_H

#include <fstream>
#include "sink/file_sink.h"


template <class T>
class file_sink : public consumer<T> {
 private:
  std::ofstream out_file;
  std::ostream * target;
  void work() override;
  void stop_worker() override { working = false; }
  std::atomic<bool> working;

 public:
  explicit file_sink(std::string & filename);
  std::string get_type() const override { return "file_sink"; }
  void run() override
  {
    working = true;
    consumer<T>::run();
  }
};

#endif  /* OUT_FILE_SINK_H */
