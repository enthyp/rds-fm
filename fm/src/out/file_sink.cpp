#include <cstring>
#include <iostream>
#include <fstream>
#include <memory>
#include <mutex>

#include "rtl-sdr.h"

#include "block.h"
#include "output.h"


class consume_samples : public task {
 private:
  std::ostream * target;
  uint32_t & buf_size;
  int16_t * buffer;
  std::mutex & buf_lock;
  std::condition_variable & buf_ready;

  int16_t ds_buf[MAXIMUM_BUFFER_LENGTH];
  downsampler ds;

public:
    output_producer(std::string filename,
                    std::ostream & target,
                    uint32_t & buf_size,
                    int16_t * buffer,
                    std::mutex & buf_lock,
                    std::condition_variable & buf_ready,
                    int m_factor)
      : buf_size {buf_size},
        buffer {buffer},
        buf_lock {buf_lock},
        buf_ready {buf_ready}
    {
        if (filename == "-") {
            this -> target = &std::cout;
        } else {
            this -> target = &target;
        }

        this -> ds = downsampler(buffer, buf_size, this -> ds_buf, m_factor);
    };

    void run() {
        while (!(this -> stop_requested())) {
            // Wait for data to appear in the buffer and save it to file.
            std::unique_lock<std::mutex> lock(this -> buf_lock);
            this -> buf_ready.wait(lock);

            if (this -> stop_requested())
                break;

            int size = this -> ds.run();
            (*(this -> target)).write(reinterpret_cast<const char *>(this -> dec_buf), size * sizeof(int16_t));
        }
    }

    void stop() {
        task::stop();
        this -> buffer_ready.notify_one();
    }
};

output_wrapper::output_wrapper(std::string filename, int m_factor) {
    // Open the output stream.
    if (filename != "-") {
        this -> target.open(filename, std::ios::binary | std::ios::out);
    }

    // Create the task to save data to file.
    this -> consume_audio = std::unique_ptr<task>(
        new output_producer(
            filename, this -> target, this -> buf_size, this -> buf,
            this -> buf_lock, this -> buf_ready, m_factor));
}
