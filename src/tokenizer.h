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
using std::shared_ptr;
using std::queue;


typedef shared_ptr<Token> TokenPtr;


class TokenizerError : public BaseException {};


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
    unique_ptr<State> state;
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


#endif // CALCXX_TOKENIZER_H
