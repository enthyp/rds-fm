#ifndef IQ_WRITER_H
#define IQ_WRITER_H

#include <basic/source.h>
#include "basic/sink.h"


class iq_writer {
private:
  std::shared_ptr<source> input;
  std::shared_ptr<sink> output;

public:
  iq_writer(std::shared_ptr<source> & input,
           std::shared_ptr<sink> & output)
   : input {input},
     output {output}
     {
        input -> to(output);
     };

  void run();
  void stop();
};

#endif  /* IQ_WRITER_H */