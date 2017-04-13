#ifndef CALCXX_EVAL_H
#define CALCXX_EVAL_H


#include <stack>
#include <string>
#include <vector>

#include "exception.h"
#include "tokens.h"


using std::stack;
using std::string;
using std::vector;


vector<Token::Ptr> extract_argument(stack<Token::Ptr> &stack, const vector<string> &spec);
bool operator_lt(const Token::Ptr &op1, const Token::Ptr &op2);


class TokensEvaluator {
public:
    void feed(const Token::Ptr &tok);
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
