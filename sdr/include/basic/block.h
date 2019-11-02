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

template <typename T>
class consumer : public block {
 public:
  virtual void receive(T * buffer, int len) = 0;
};

template <typename T>
class producer : public block {
 public:
  virtual void to(std::shared_ptr<consumer<T>> b) = 0;
};


#endif  /* BLOCK_H */
