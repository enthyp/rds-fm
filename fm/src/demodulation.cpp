#include <cstring>
#include "block.h"
#include "demodulation.h"


class demodulation : public task {
 private:
  std::shared_ptr<block> sink;

  uint32_t & buf_size;
  int16_t * buffer;
  std::mutex & buffer_lock;
  std::condition_variable & buffer_ready;

  decimator dec;
  int16_t lp_buf[MAXIMUM_BUFFER_LENGTH];

  int dm_start = 0;
  int32_t dm_acc = 0;
  int16_t dm_buf[MAXIMUM_BUFFER_LENGTH];

  downsampler ds;

 public:
  demodulation(std::shared_ptr<block> sink,
               uint32_t & buf_size,
               int16_t * buffer,
               std::mutex & buffer_lock,
               std::condition_variable & buffer_ready)
    : sink {sink},
      buf_size {buf_size},
      buffer {buffer},
      buffer_lock {buffer_lock},
      buffer_ready {buffer_ready}
      {
        this -> dec = decimator(buffer, buf_size, sink.get(), 10, 10);
        this -> ds = downsampler(buffer, buf_size, sink.get(), 5);
      };

  void run() {
    while (!stop_requested()) {
      // Wait for data in the buffer.
      std::unique_lock<std::mutex> lock(this -> buffer_lock);
      this -> buffer_ready.wait(lock);

      if (this -> stop_requested())
        break;

      // TODO: process the data...
      this -> dec.run();


      // And send the data to output block.
      this -> sink -> receive(this -> buffer, this -> buf_size);
    }
  }

  void stop() {
    task::stop();
    this -> buffer_ready.notify_one();
  }

  void fm_demodulate(int16_t * in_buf, uint32_t len, int16_t out_buffer) {

  }
};

demodulator::demodulator() {
  this -> demodulate = std::unique_ptr<task>(new demodulation(this -> sink, this -> buf_size, this -> buf,
                                                              this -> buffer_lock, this -> buffer_ready));
}

void demodulator::to(std::shared_ptr<block> b) {
    this -> sink = b;
}

void demodulator::run() {
    this -> worker_t = std::thread(std::ref(*(this -> demodulate)));
}

void demodulator::stop() {
    this -> demodulate -> stop();
    this -> worker_t.join();
}

void demodulator::receive(uint16_t * buffer, uint32_t len) {
    std::lock_guard<std::mutex> lock(this -> buffer_lock);
    std::memcpy(this -> buf, buffer, sizeof(int16_t) * len);
    this -> buf_size = len;
    this -> buffer_ready.notify_one();
}