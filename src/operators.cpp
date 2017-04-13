#include <cassert>
#include <limits>
#include <memory>
#include <numeric>

#include "operators.h"


using std::accumulate;
using std::dynamic_pointer_cast;
using std::make_shared;
using std::numeric_limits;


map<TokenType, OperatorFunc> g_builtin_operator_table = {
    {TokenType::PLUS, op_add},
    {TokenType::MINUS, op_sub},
    {TokenType::MULT, op_mult},
    {TokenType::DIV, op_div}
};


static double token_to_num(const Token::Ptr &tok) {
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


Token::Ptr op_add(const vector<Token::Ptr> &args) {
    assert(args.size() > 0);
    double sum = accumulate(
        args.begin(), args.end(), 0.0,
        [](double result, Token::Ptr tok) {
            return result + token_to_num(tok);
        });
    return best_num_token(sum, args);
}

Token::Ptr op_sub(const vector<Token::Ptr> &args) {
    double v1 = 0, v2 = 0;
    if (args.size() == 1) {
        v1 = 0;
        v2 = token_to_num(args[0]);
    } else if (args.size() == 2) {
        v1 = token_to_num(args[0]);
        v2 = token_to_num(args[1]);
    } else {
        assert(!"Unreachable");
    }
    return best_num_token(v1 - v2, args);
}

Token::Ptr op_mult(const vector<Token::Ptr> &args) {
    assert(args.size() == 2);
    double v1 = token_to_num(args[0]);
    double v2 = token_to_num(args[1]);
    return best_num_token(v1 * v2, args);
}

Token::Ptr op_div(const vector<Token::Ptr> &args) {
    assert(args.size() == 2);
    double v1 = token_to_num(args[0]);
    double v2 = token_to_num(args[1]);
    if (v2 == 0.0) {
        double result = std::numeric_limits<double>::infinity();
        return make_shared<TokenFloat>(result);
    } else {
        double result = v1 / v2;
        if (is_tokens_all_int(args) && (int64_t)v1 % (int64_t)v2 == 0) {
            return make_shared<TokenInt>(result);
        } else {
            return make_shared<TokenFloat>(result);
        }
    }
}
