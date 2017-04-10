#ifndef CALCXX_EVAL_H
#define CALCXX_EVAL_H


#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <stack>
#include <string>
#include <vector>
#include "exception.h"
#include "tokens.h"


using std::function;
using std::map;
using std::shared_ptr;
using std::stack;
using std::string;
using std::vector;


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


using OperatorFunction = function<void(stack<Token::Ptr > &)>;

void operator_add(stack<Token::Ptr> &stack);
void operator_sub(stack<Token::Ptr> &stack);
void operator_mult(stack<Token::Ptr> &stack);
void operator_div(stack<Token::Ptr> &stack);


vector<Token::Ptr> extract_argument(stack<Token::Ptr> &stack, const vector<string> &spec);
bool operator_lt(Token::Ptr op1, Token::Ptr op2);


class TokensEvaluator {
public:
    void feed(Token::Ptr tok);
    Token::Ptr get_result();;
    void reset();

    bool is_finished() const {
        return this->ops.size() == 0 && this->values.size() == 1;
    }

private:
    stack<Token::Ptr> ops;
    stack<Token::Ptr> values;

    void eval_top();
    void check_result();
};


#endif //CALCXX_EVAL_H
