#include <algorithm>
#include <cassert>
#include <limits>
#include <memory>
#include <stdexcept>
#include "eval.h"
#include "utils.hpp"


using std::dynamic_pointer_cast;
using std::make_shared;
using std::out_of_range;
using std::reverse;


static map<TokenType, OperatorFunction> g_operator_table = {
    {TokenType::PLUS, operator_add},
    {TokenType::MINUS, operator_sub},
    {TokenType::MULT, operator_mult},
    {TokenType::DIV, operator_div}
};
static map<TokenType, int> g_operator_precedence = {
    {TokenType::LPAR, -1},
    {TokenType::PLUS, 1},
    {TokenType::MINUS, 1},
    {TokenType::MULT, 2},
    {TokenType::DIV, 2},
    {TokenType::RPAR, -2},
    {TokenType::END, -3}
};


void TokensEvaluator::feed(Token::Ptr tok) {
    if (*tok == Token(TokenType::LPAR)) {
        this->ops.push(tok);
    } else if (tok->is_op()) {
        while (!this->ops.empty() && !operator_lt(this->ops.top(), tok)) {
            if (this->ops.top()->type == TokenType::LPAR) {
                if (tok->type != TokenType::RPAR) {
                    throw EvalError("Unclosed left parenthesis.");
                }
                this->ops.pop();
                return;
            } else {
                this->eval_top();
            }
        }

        if (tok->type == TokenType::END) {
            this->check_result();
        } else {
            this->ops.push(tok);
        }
    } else {
        this->values.push(tok);
    }
}

void TokensEvaluator::eval_top() {
    assert(!this->ops.empty());
    Token::Ptr tok = this->ops.top();
    this->ops.pop();
    auto it = g_operator_table.find(tok->type);
    if (it != g_operator_table.end()) {
        (it->second)(this->values);
    } else {
        throw NotImplementedOperation(string(1, static_cast<char>(tok->type)));
    }
}

void TokensEvaluator::check_result() {
    assert(this->ops.empty());
    if (this->values.size() == 0) {
        throw EvalError("No result");
    }
}

Token::Ptr TokensEvaluator::get_result() {
    if (!this->is_finished()) {
        throw EvalError("Not finished");
    }
    Token::Ptr tok = this->values.top();
    this->values.pop();
    return tok;
}

void TokensEvaluator::reset() {
    stack<Token::Ptr>().swap(this->ops);
    stack<Token::Ptr>().swap(this->values);
}


bool operator_lt(Token::Ptr op1, Token::Ptr op2) {
    try {
        return g_operator_precedence.at(op1->type) < g_operator_precedence.at(op2->type);
    } catch (const out_of_range &) {
        throw EvalError("Operation precedence unknown");
    }
}


static double token_to_num(Token::Ptr tok) {
    auto ti = dynamic_pointer_cast<TokenInt>(tok);
    if (ti) {
        return ti->value;
    } else {
        auto tf = dynamic_pointer_cast<TokenFloat>(tok);
        assert(tf);
        return tf->value;
    }
}


static bool is_tokens_all_int(const vector<Token::Ptr> &args) {
    for (auto tok : args) {
        if (!dynamic_pointer_cast<TokenInt>(tok)) {
            return false;
        }
    }
    return true;
}


static Token::Ptr best_num_token(double num, const vector<Token::Ptr> &args) {
    if (is_tokens_all_int(args)) {
        return make_shared<TokenInt>(num);
    } else {
        return make_shared<TokenFloat>(num);
    }
}


void operator_add(stack<Token::Ptr> &stack) {
    auto args = extract_argument(stack, {"if", "if"});
    double v1 = token_to_num(args[0]);
    double v2 = token_to_num(args[1]);
    double result = v1 + v2;
    stack.push(best_num_token(result, args));
}


void operator_sub(stack<Token::Ptr> &stack) {
    auto args = extract_argument(stack, {"if", "if"});
    double v1 = token_to_num(args[0]);
    double v2 = token_to_num(args[1]);
    double result = v1 - v2;
    stack.push(best_num_token(result, args));
}


void operator_mult(stack<Token::Ptr> &stack) {
    auto args = extract_argument(stack, {"if", "if"});
    double v1 = token_to_num(args[0]);
    double v2 = token_to_num(args[1]);
    double result = v1 * v2;
    stack.push(best_num_token(result, args));
}


void operator_div(stack<Token::Ptr> &stack) {
    auto args = extract_argument(stack, {"if", "if"});
    double v1 = token_to_num(args[0]);
    double v2 = token_to_num(args[1]);
    if (v2 == 0.0) {
        double result = std::numeric_limits<double>::infinity();
        stack.push(make_shared<TokenFloat>(result));
    } else {
        double result = v1 / v2;
        if (is_tokens_all_int(args) && (int64_t)v1 % (int64_t)v2 == 0) {
            stack.push(make_shared<TokenInt>(result));
        } else {
            stack.push(make_shared<TokenFloat>(result));
        }
    }
}


vector<Token::Ptr>
extract_argument(stack<Token::Ptr> &stack, const vector<string> &spec) {
    if (stack.size() < spec.size()) {
        throw ArgumentError(
            "missing argument, expected " + repr(spec.size()) + " argument"
                + ", only " + repr(stack.size()) + " argument available\n"
        );
    }

    vector<Token::Ptr> ans;
    for (string s : spec) {
        Token::Ptr tok = stack.top();
        stack.pop();
        char type = static_cast<char>(tok->type);
        if (s.find(type) != string::npos) {
            ans.push_back(tok);
        } else {
            throw ArgumentError(
                "argument type mismatch, expected '" + s + "', got " + string(1, type) + "\n"
            );
        }
    }
    reverse(ans.begin(), ans.end());
    return ans;
}
