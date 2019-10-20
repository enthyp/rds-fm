#include <cstring>
#include <iostream>
#include <fstream>
#include <memory>
#include <mutex>

#include "rtl-sdr.h"

#include "block.h"
#include "output.h"


class output_producer : public task {
private:
    std::ostream * target;
    uint32_t & buf_size;
    int16_t * buffer;
    std::mutex & buffer_lock;
    std::condition_variable & buffer_ready;

public:
    output_producer(
            std::string filename,
            std::ostream & target,
            uint32_t & buf_size,
            int16_t * buffer,
            std::mutex & buffer_lock,
            std::condition_variable & buffer_ready) :
            buf_size {buf_size},
            buffer {buffer},
            buffer_lock {buffer_lock},
            buffer_ready {buffer_ready} {
        if (filename == "-") {
            this -> target = &std::cout;
        } else {
            this -> target = &target;
        }
    };

    void run() {
        while (!(this -> stop_requested())) {
            // Wait for data to appear in the buffer and save it to file.
            std::unique_lock<std::mutex> lock(this -> buffer_lock);
            this -> buffer_ready.wait(lock);

            if (this -> stop_requested())
                break;

            (*(this -> target)).write(reinterpret_cast<const char *>(this -> buffer), this -> buf_size * sizeof(int16_t));
        }
    }

    void stop() {
        task::stop();
        this -> buffer_ready.notify_one();
    }
};

output_wrapper::output_wrapper(std::string filename) {
    // Open the output stream.
    if (filename != "-") {
        this -> target.open(filename, std::ios::binary | std::ios::out);
    }

    // Create the task to save data to file.
    this -> emit_samples = std::unique_ptr<task>(new output_producer(filename, this -> target, this -> buf_size, this -> buf,
            this -> buffer_lock, this -> buffer_ready));
}

void output_wrapper::run() {
    this -> worker_t = std::thread(std::ref(*(this -> emit_samples)));
}

void output_wrapper::stop() {
    // Prompt the producer thread to stop and wait.
    this -> emit_samples -> stop();
    this -> worker_t.join();
}

void output_wrapper::receive(int16_t * buffer, uint32_t len) {
    std::lock_guard<std::mutex> lock(this -> buffer_lock);
    std::memcpy(this -> buf, buffer, sizeof(int16_t) * len);
    this -> buf_size = len;
    this -> buffer_ready.notify_one();
}
