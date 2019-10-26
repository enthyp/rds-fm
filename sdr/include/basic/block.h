#ifndef BLOCK_H
#define BLOCK_H

#include <memory>

#define DEFAULT_BUFFER_LENGTH 16384
#define MAXIMUM_BUFFER_LENGTH (16 * DEFAULT_BUFFER_LENGTH)


class block {
public:
    virtual std::string get_type() const = 0;
    virtual void run() = 0;
    virtual void stop() = 0;
};

class consumer : public block {
 public:
  virtual void receive(int16_t * buffer, int len) = 0;
};

class producer : public block {
 public:
  virtual void to(std::shared_ptr<consumer> b) = 0;
};


#endif  /* BLOCK_H */
