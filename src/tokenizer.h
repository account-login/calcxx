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


using std::string;
using std::tuple;
using std::unique_ptr;
using std::queue;


class State {
public:
    virtual tuple<Token::Ptr, bool, State*> feed(char ch) = 0;

protected:
    tuple<Token::Ptr, bool, State*> keep_state();
};


class InitState : public State {
public:
    virtual tuple<Token::Ptr, bool, State*> feed(char ch);
};


enum class NumberSubState {
    init,
    int_digit,
    dotted,
    leading_dot,
    exp,
    exp_signed,
    exp_digit,
};


class NumberState : public State {
public:
    virtual tuple<Token::Ptr, bool, State*> feed(char ch);

private:
    NumberSubState state = NumberSubState::init;
    string int_digits;
    string dot_digits;
    string exp_digits;
    int exp_sign = 1;
    bool has_dot = false;

    tuple<Token::Ptr, bool, State*> finish();
};


class Tokenizer {
public:
    Tokenizer() : state(new InitState()) {}
    void feed(char ch);
    Token::Ptr pop();
    void reset();

private:
    unique_ptr<State> state;
    queue<Token::Ptr> tokens;
    SourcePos start_pos = SourcePos();
    SourcePos prev_pos;
    SourcePos cur_pos = SourcePos();

    void set_new_state(State *new_state);
};


#endif // CALCXX_TOKENIZER_H
