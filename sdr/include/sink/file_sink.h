#ifndef OUT_FILE_SINK_H
#define OUT_FILE_SINK_H

#include <fstream>
#include "base.h"


template <class T>
class file_sink : public consumer<T> {
 private:
  std::ofstream out_file;
  std::ostream * target;
  std::atomic<bool> working;

  void worker() override;
  void stop_worker() override { working = false; }

 public:
  explicit file_sink(std::string & filename);
  void run() override
  {
    working = true;
    consumer<T>::run();
  }
};

#endif  /* OUT_FILE_SINK_H */
