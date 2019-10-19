#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <memory>
#include "errors.h"

#define DEFAULT_BUFFER_LENGTH 16384
#define MAXIMUM_BUFFER_LENGTH DEFAULT_BUFFER_LENGTH


class block {
public:
    virtual std::string get_type() const = 0;

    virtual void from(std::shared_ptr<block> b) {
        throw block_conn_exception(b -> get_type(), this -> get_type());
    };

    virtual void to(std::shared_ptr<block> b) {
        throw block_conn_exception(this -> get_type(), b -> get_type());
    };

    virtual void run() = 0;

    virtual void stop() = 0;

    virtual void receive(uint16_t * buffer, uint32_t len) {
        throw bad_call_exception(this -> get_type(), "receive");
    }
};

#endif  /* BLOCK_H */
