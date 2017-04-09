#ifndef CALCXX_TOKENIZER_H
#define CALCXX_TOKENIZER_H

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <memory>
#include <queue>
#include <string>
#include <tuple>
#include "exception.h"
#include "sourcepos.h"
#include "tokens.h"
#include "utils.hpp"


using std::tie;
using std::to_string;
using std::tuple;
using std::unique_ptr;
using std::queue;


class TokenizerError : public BaseException {};


class State {
public:
    virtual tuple<Token::Ptr, bool, State*> feed(char ch) = 0;
};


class InitState : public State {
public:
    virtual tuple<Token::Ptr, bool, State*> feed(char ch);
};


class NumberState : public State {
public:
    virtual tuple<Token::Ptr, bool, State*> feed(char ch);

private:
    string digits;
};


class Tokenizer {
public:
    Tokenizer() : state(new InitState()) {}
    void feed(char ch);
    Token::Ptr pop();

private:
    unique_ptr<State> state;
    queue<Token::Ptr> tokens;
    SourcePos start_pos = SourcePos();
    SourcePos prev_pos;
    SourcePos cur_pos = SourcePos();

    inline void set_new_state(State *new_state) {
        if (new_state != this->state.get()) {
            this->state.reset(new_state);
        }
    }
};


#endif // CALCXX_TOKENIZER_H
