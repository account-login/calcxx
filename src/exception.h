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
    explicit BaseException(const string &msg = "BaseException") : msg(msg) {}
    virtual const char *what() const throw();
};


class TokenizerError : public BaseException {
public:
    explicit TokenizerError(const string &msg) : BaseException(msg) {}
};

class ParserError : public BaseException {
public:
    explicit ParserError(const string &msg) : BaseException(msg) {};
};

class EvalError : public BaseException {
public:
    explicit EvalError(const string &msg) : BaseException(msg) {};
};

class ArgumentError : public EvalError {
public:
    explicit ArgumentError(const string &msg) : EvalError(msg) {};
};

class NotImplementedOperation : public EvalError {
public:
    explicit NotImplementedOperation(const string &msg) : EvalError(msg) {}
};

#endif //CALCXX_EXCEPTION_H
