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


class TokenizerError : public BaseException {
public:
    TokenizerError(const string &msg) : BaseException(msg) {}
};

class ParserError : public BaseException {
public:
    ParserError(const string &msg) : BaseException(msg) {};
};

class EvalError : public BaseException {
public:
    EvalError(const string &msg) : BaseException(msg) {};
};

class ArgumentError : public EvalError {
public:
    ArgumentError(const string &msg) : EvalError(msg) {};
};

class NotImplementedOperation : public EvalError {
public:
    NotImplementedOperation(const string &msg) : EvalError(msg) {}
};

#endif //CALCXX_EXCEPTION_H
