#include "block.h"
#include "demodulation.h"

void demodulator::to(std::shared_ptr<block> b) {
    this -> sink = b;
}

void demodulator::run() {

}

void demodulator::stop() {

}

void receive(uint16_t * buffer, uint32_t len) {

}