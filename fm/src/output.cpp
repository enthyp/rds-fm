#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>

#include "rtl-sdr.h"

#include "block.h"
#include "output.h"


class output_producer : public task {
private:
    std::ofstream & file;
    uint32_t & buf_size;
    uint16_t * buffer;
    std::mutex & buffer_lock;
    std::condition_variable & buffer_ready;

public:
    output_producer(std::ofstream & file, uint32_t & buf_size, uint16_t * buffer,
            std::mutex & buffer_lock, std::condition_variable & buffer_ready) :
            file {file}, buf_size {buf_size}, buffer {buffer},
            buffer_lock {buffer_lock}, buffer_ready {buffer_ready} {};

    void run() {
        while (true) {
            // Wait for data to appear in the buffer and save it to file.
            std::unique_lock<std::mutex> lock(this -> buffer_lock);
            this -> buffer_ready.wait(lock);

            for (int i = 0; i < (int)this -> buf_size; i++) {
                this -> file << (unsigned char) this -> buffer[i];
                this -> file << (unsigned char) (this -> buffer[i] >> 8);
            }
        }
    }
};

output_wrapper::output_wrapper(std::string filename) {
    // Open the file.
    this -> file.open(filename, std::ios::out | std::ios::binary);

    // Create the task to save data to file.
    this -> emit_samples = std::unique_ptr<task>(new output_producer(this -> file, this -> buf_size, this -> buf,
            this -> buffer_lock, this -> buffer_ready));
}

void output_wrapper::from(std::shared_ptr<block> b) {
    this -> source = b;
}

void output_wrapper::run() {
    this -> worker_t = std::thread(std::ref(*(this -> emit_samples)));
}

void output_wrapper::stop() {
    // Prompt the producer thread to stop and wait.
    this -> emit_samples -> stop();
    this -> worker_t.join();

    // Close the file.
    this -> file.close();
}

void output_wrapper::receive(uint16_t * buffer, uint32_t len) {
    std::lock_guard<std::mutex> lock(this -> buffer_lock);
    std::memcpy(this -> buf, buffer, len);
    this -> buf_size = len;
    this -> buffer_ready.notify_one();
}