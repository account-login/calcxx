#ifndef CALCXX_TOKENS_H
#define CALCXX_TOKENS_H


#include <memory>
#include <string>
#include "sourcepos.h"
#include "utils.hpp"


using std::shared_ptr;
using std::string;
using std::to_string;


enum class TokenType {
    INT = 'i',
    FLOAT = 'f',

    PLUS = '+',
    MINUS = '-',
    MULT = '*',
    DIV = '/',

    LPAR = '(',
    RPAR = ')',

    END = '$',
};


REPR(TokenType) {
    return string(1, static_cast<char>(value));
}


struct Token {
    using Ptr = shared_ptr<Token>;

    TokenType type;
    SourcePos start;
    SourcePos end;

    explicit Token(TokenType type) : type(type) {}

    virtual inline bool is_op() const {
        return true;
    }

    virtual inline bool operator==(const Token &other) const {
        return this->type == other.type;
    }

    virtual inline string _token_name() const {
        return "Token:" + string(1, static_cast<char>(this->type));
    }

    virtual inline string _repr_value() const {
        return "";
    }
};


REPR(Token) {
    return "<" + value._token_name() + " "
    + value._repr_value() + " "
    + "start=" + repr(value.start) + " end=" + repr(value.end) + ">";
}


struct TokenInt : Token {
    int64_t value;

    inline explicit TokenInt(int64_t value)
        : Token(TokenType::INT), value(value)
    {}

    virtual inline bool is_op() const {
        return false;
    }

    virtual inline bool operator==(const Token &other) const {
        return this->type == other.type
            && this->value == reinterpret_cast<const TokenInt &>(other).value;
    }

    virtual inline string _token_name() const {
        return "Int";
    }

    virtual inline string _repr_value() const {
        return to_string(this->value);
    }
};


struct TokenFloat : Token {
    double value;

    explicit TokenFloat(double value)
        : Token(TokenType::FLOAT), value(value)
    {}

    virtual inline bool is_op() const {
        return false;
    }

    virtual inline bool operator==(const Token &other) const {
        return this->type == other.type
            && this->value == reinterpret_cast<const TokenFloat &>(other).value;
    }

    virtual inline string _token_name() const {
        return "Float";
    }

    virtual inline string _repr_value() const {
        return to_string(this->value);
    }
};


#endif //CALCXX_TOKENS_H
