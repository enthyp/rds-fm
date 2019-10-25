#ifndef DEMODULATION_H
#define DEMODULATION_H

#include <string>
#include <memory>
#include "blocks/flow.h"
#include "blocks/task.h"


class demodulator : public flow {
  // TODO: private demodulation params...
public:
    demodulator();
    std::string get_type() const { return "demodulator"; }
    void to(std::shared_ptr<block> b);
    void run();
    void stop();
    void receive(uint16_t * buffer, uint32_t len);
};

#endif  /* DEMODULATION_H */
