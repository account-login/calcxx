#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <exception>
#include <memory>
#include <queue>
#include <string>
#include <tuple>
#include "utils.hpp"


using namespace std;


struct SourcePos {
    int lineno;
    int rowno;

private:
    bool last_newline = true;

public:
    inline SourcePos(int lineno, int rowno)
        : lineno(lineno), rowno(rowno)
    {}

    // invalid
    inline SourcePos() : lineno(-1), rowno(-1) {}

    inline bool operator == (const SourcePos &other) const {
        return tie(this->lineno, this->rowno) == tie(other.lineno, other.rowno);
    }

    inline bool operator != (const SourcePos &other) const {
        return !(*this == other);
    }

    inline bool is_valid() const {
        return this->lineno >= 0 && this->rowno >= 0;
    }

    void add_char(char ch);
};


REPR(SourcePos) {
    return "<Pos " + to_string(value.lineno) + ":" + to_string(value.rowno) + ">";
}


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
    TokenType type;
    SourcePos start;
    SourcePos end;

    explicit Token(TokenType type) : type(type) {}

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


typedef shared_ptr<Token> TokenPtr;


class TokenizerError : public exception {
private:
    string msg;

public:
    inline TokenizerError(const string &msg = "TokenizerError") : msg(msg) {}

    inline const char *what() const throw() {
        return this->msg.data();
    }
};


class State {
public:
    virtual tuple<TokenPtr, bool, State*> feed(char ch) = 0;
};


class InitState : public State {
public:
    virtual tuple<TokenPtr, bool, State*> feed(char ch);
};


class NumberState : public State {
public:
    virtual tuple<TokenPtr, bool, State*> feed(char ch);

private:
    string digits;
};


class Tokenizer {
public:
    Tokenizer() : state(new InitState()) {}
    void feed(char ch);
    TokenPtr pop();

private:
    shared_ptr<State> state;
    queue<TokenPtr> tokens;
    SourcePos start_pos = SourcePos();
    SourcePos prev_pos;
    SourcePos cur_pos = SourcePos();

    inline void set_new_state(State *new_state) {
        if (new_state != this->state.get()) {
            this->state.reset(new_state);
        }
    }
};


#endif // TOKENIZER_H
