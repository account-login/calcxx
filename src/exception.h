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
    BaseException(const string &msg = "BaseException") : msg(msg) {}
    virtual const char *what() const throw();
};


#endif //CALCXX_EXCEPTION_H
