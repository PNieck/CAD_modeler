#ifndef EXCEPTION_WITH_MSG_H
#define EXCEPTION_WITH_MSG_H

#include <exception>


class ExceptionWithMsg : public std::exception {
public:
    ExceptionWithMsg(char * msg) : message(msg) {}

    const char * what() const override {
        return message;
    }

private:
    char * message;
};

#endif