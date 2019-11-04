#ifndef BLOCKS_FLOW_H
#define BLOCKS_FLOW_H

#include <iostream>
#include <cstring>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "basic/block.h"
#include "ring_buffer.h"


// TODO: I guess we could get rid off templates for now?
template <typename T_in, typename T_out>
class flow : public producer<T_out>, public consumer<T_in> {
 protected:
  virtual void work() override {
    while (working) {
      // Wait for data in the buffer.
      std::unique_lock<std::mutex> lock(flow<T_in, T_out>::buf_lock);
      if (!flow<T_in, T_out>::read_ready) {
        flow<T_in, T_out>::read_ready_cond.wait(lock, [this] { return this -> read_ready; });
      }

      if (!working)
        break;

      int len = decimate();
      count += len;
      // And send the data to output block.
      flow<T_in, T_out>::succ -> receive(decimated_buffer, len);

      flow<T_in, T_out>::read_ready = false;
      lock.unlock();
    }
  };
};


#endif  /* BLOCKS_FLOW_H */
