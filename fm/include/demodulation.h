#ifndef DEMODULATION_H
#define DEMODULATION_H

#include <string>
#include <memory>
#include "block.h"


class demodulator : public block {
private:
    std::shared_ptr <block> source;
    std::shared_ptr <block> sink;

public:
    std::string get_type() const { return "demodulator"; }
    void to(std::shared_ptr<block> b);
    void run();
    void stop();
    void receive(uint16_t * buffer, uint32_t len);
};

#endif  /* DEMODULATION_H */
