#ifndef CALCXX_EXCEPTION_H
#define CALCXX_EXCEPTION_H


#include <exception>
#include <string>


using std::exception;
using std::string;


class BaseException : public exception {
protected:
    string msg;

public:
    inline BaseException(const string &msg = "BaseException") : msg(msg) {}

    virtual inline const char *what() const throw() {
        return this->msg.data();
    }
};


#endif //CALCXX_EXCEPTION_H
