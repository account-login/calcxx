#ifndef CALCXX_OPERATORS_H
#define CALCXX_OPERATORS_H


#include <functional>
#include <map>
#include <vector>
#include "tokens.h"


using std::function;
using std::map;
using std::vector;


typedef function<Token::Ptr (const vector<Token::Ptr> &)> OperatorFunc;
extern map<TokenType, OperatorFunc> g_builtin_operator_table;


Token::Ptr op_add(const vector<Token::Ptr> &args);
Token::Ptr op_sub(const vector<Token::Ptr> &args);
Token::Ptr op_mult(const vector<Token::Ptr> &args);
Token::Ptr op_div(const vector<Token::Ptr> &args);


#endif //CALCXX_OPERATORS_H
