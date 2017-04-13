#include <algorithm>
#include <cassert>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "eval.h"
#include "operators.h"
#include "utils.hpp"


using std::map;
using std::out_of_range;
using std::reverse;
using std::string;
using std::vector;


static map<TokenType, vector<string>> g_operator_sigs = {
    {TokenType::PLUS, {"if", "if"}},
    {TokenType::MINUS, {"if", "if"}},
    {TokenType::MULT, {"if", "if"}},
    {TokenType::DIV, {"if", "if"}}
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


void TokensEvaluator::feed(const Token::Ptr &tok) {
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
    auto it = g_builtin_operator_table.find(tok->type);
    if (it != g_builtin_operator_table.end()) {
        auto func = it->second;
        const vector<string> &sigs = g_operator_sigs.find(tok->type)->second;
        vector<Token::Ptr> args = extract_argument(this->values, sigs);
        Token::Ptr result = func(args);
        this->values.push(result);
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


bool operator_lt(const Token::Ptr &op1, const Token::Ptr &op2) {
    try {
        return g_operator_precedence.at(op1->type) < g_operator_precedence.at(op2->type);
    } catch (const out_of_range &) {
        throw EvalError("Operation precedence unknown");
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
