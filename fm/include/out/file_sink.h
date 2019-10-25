#ifndef OUT_FILE_SINK_H
#define OUT_FILE_SINK_H

#include <fstream>
#include "../blocks/sink.h"


class output_wrapper : public sink {
 private:
  std::ofstream target;

 public:
  output_wrapper(std::string filename, int m_factor);
  std::string get_type() const { return "output_wrapper"; }
  void run();
  void stop();
}

#endif  /* OUT_FILE_SINK_H */
