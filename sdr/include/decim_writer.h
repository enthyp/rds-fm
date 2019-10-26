#ifndef DECIM_WRITER_H
#define DECIM_WRITER_H

#include <basic/source.h>
#include <basic/flow.h>
#include "basic/sink.h"


class decim_writer {
private:
  std::shared_ptr<source> input;
  std::shared_ptr<flow> decimator;
  std::shared_ptr<sink> output;

public:
  decim_writer(std::shared_ptr<source> & input,
               std::shared_ptr<flow> & decimator,
               std::shared_ptr<sink> & output)
   : input {input},
     decimator {decimator},
     output {output}
     {
        input -> to(decimator);
        decimator -> to(output);
     };

  void run();
  void stop();
};

#endif  /* DECIM_WRITER_H */