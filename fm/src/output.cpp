#include <memory>
#include <mutex>

#include "rtl-sdr.h"

#include "block.h"
#include "output.h"


void output_wrapper::from(std::shared_ptr<block> b) {
    this -> source = b;
}

void output_wrapper::run() {
    this -> worker_t = std::thread(dump);
}

void output_wrapper::stop() {

}

void receive(uint16_t * buffer, uint32_t len) {
    // TODO: needs locks!
    std::lock_guard<std::mutex> lock(this -> buffer_lock);
    std::memcpy(this -> buf, buffer, len);
}