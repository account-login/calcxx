#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <memory>
#include <string>
#include <tuple>

#include "tokenizer.h"


using std::atoi;
using std::atoll;
using std::pow;
using std::make_shared;
using std::numeric_limits;
using std::string;
using std::tie;


tuple<Token::Ptr, bool, State *> State::keep_state() {
    return make_tuple(Token::Ptr(), true, this);
}


tuple<Token::Ptr, bool, State*> InitState::feed(char ch) {
#define SINGLE_CHAR(tok_ch, tok_type) \
    else if (ch == tok_ch) { \
        Token::Ptr tok = make_shared<Token>(TokenType::tok_type); \
        return make_tuple(tok, true, this); \
    }

    if (ch == '\0') {
        Token::Ptr tok = make_shared<Token>(TokenType::END);
        return make_tuple(tok, true, this);
    } else if (isspace(ch)) {
        return this->keep_state();
    }
    SINGLE_CHAR('+', PLUS)
    SINGLE_CHAR('-', MINUS)
    SINGLE_CHAR('*', MULT)
    SINGLE_CHAR('/', DIV)
    SINGLE_CHAR('(', LPAR)
    SINGLE_CHAR(')', RPAR)
    else if (isdigit(ch) || ch == '.') {
        return make_tuple(Token::Ptr(), false, new NumberState);
    } else {
        throw TokenizerError("Unknown char: " + string(1, ch));
    }

#undef SINGLE_CHAR
}


tuple<Token::Ptr, bool, State*> NumberState::feed(char ch) {
    switch (this->state) {
    case NumberSubState::init:
        if (isdigit(ch)) {
            this->int_digits.push_back(ch);
            this->state = NumberSubState::int_digit;
            return this->keep_state();
        } else if (ch == '.') {
            this->has_dot = true;
            this->state = NumberSubState::leading_dot;
            return this->keep_state();
        } else {
            throw TokenizerError("Unknown char: " + string(1, ch) + ", expect dot or digit");
        }
        break;
    case NumberSubState::int_digit:
        if (isdigit(ch)) {
            this->int_digits.push_back(ch);
            return this->keep_state();
        } else if (ch == '.') {
            this->has_dot = true;
            this->state = NumberSubState::dotted;
            return this->keep_state();
        } else if (ch == 'e' || ch == 'E') {
            this->state = NumberSubState::exp;
            return this->keep_state();
        } else {
            return this->finish();
        }
        break;
    case NumberSubState::leading_dot:
        if (isdigit(ch)) {
            this->dot_digits.push_back(ch);
            this->state = NumberSubState::dotted;
            return this->keep_state();
        } else {
            throw TokenizerError("Unknown char: " + string(1, ch) + ", expect digit");
        }
        break;
    case NumberSubState::dotted:
        if (isdigit(ch)) {
            this->dot_digits.push_back(ch);
            return this->keep_state();
        } else if (ch == 'e' || ch == 'E') {
            this->state = NumberSubState::exp;
            return this->keep_state();
        } else {
            return this->finish();
        }
        break;
    case NumberSubState::exp:
        if (ch == '+' || ch == '-') {
            this->state = NumberSubState::exp_signed;
            if (ch == '-') {
                this->exp_sign = -1;
            }
            return this->keep_state();
        } else if (isdigit(ch)) {
            this->exp_digits.push_back(ch);
            this->state = NumberSubState::exp_digit;
            return this->keep_state();
        } else {
            throw TokenizerError("Unknown char: " + string(1, ch) + ", expect digit or sign");
        }
        break;
    case NumberSubState::exp_signed:
        if (isdigit(ch)) {
            this->exp_digits.push_back(ch);
            this->state = NumberSubState::exp_digit;
            return this->keep_state();
        } else {
            throw TokenizerError("Unknown char: " + string(1, ch) + ", expect digit");
        }
        break;
    case NumberSubState::exp_digit:
        if (isdigit(ch)) {
            this->exp_digits.push_back(ch);
            return this->keep_state();
        } else {
            return this->finish();
        }
        break;
    }
    assert(!"Unreachable");
}


tuple<Token::Ptr, bool, State *> NumberState::finish() {
    int64_t iv = atoll(this->int_digits.data());
    double dv = iv;

    double div = 10;
    for (char ch : this->dot_digits) {
        dv += (ch - '0') / div;
        div *= 10;
    }

    if (!this->exp_digits.empty()) {
        int exp = atoi(this->exp_digits.data()) * this->exp_sign;
        dv *= pow(10, exp);
        iv *= pow(10, exp);
    }

    Token::Ptr tok;
    if (!this->has_dot && this->exp_sign > 0 && numeric_limits<int64_t>::max() > dv) {
        tok = make_shared<TokenInt>(iv);
    } else {
        tok = make_shared<TokenFloat>(dv);
    }

    return make_tuple(tok, false, new InitState);
}


void Tokenizer::feed(char ch) {
    Token::Ptr tok;
    bool eaten = false;
    State *new_state;

    this->prev_pos = this->cur_pos;
    this->cur_pos.add_char(ch);

    while (!eaten) {
        tie(tok, eaten, new_state) = this->state->feed(ch);
        if (!this->start_pos.is_valid()) {
            // ch got eaten by non-init state
            if ((eaten && !dynamic_cast<InitState *>(this->state.get()))
                || tok) // or single char token
            {
                this->start_pos = this->cur_pos;
            }
        }

        if (tok) {
            assert(this->start_pos.is_valid());
            tok->start = this->start_pos;
            if (eaten) {
                tok->end = this->cur_pos;
            } else {
                tok->end = this->prev_pos;
            }
            this->start_pos = SourcePos();  // invalidate start_pos
            this->tokens.push(tok);
        }

        this->set_new_state(new_state);
    }
}

Token::Ptr Tokenizer::pop() {
    if (!this->tokens.empty()) {
        Token::Ptr ret = this->tokens.front();
        this->tokens.pop();
        return ret;
    } else {
        return Token::Ptr();
    }
}

void Tokenizer::reset() {
    this->set_new_state(new InitState);
    this->start_pos = SourcePos();
    this->cur_pos = SourcePos();
    queue<Token::Ptr>().swap(this->tokens);
}

void Tokenizer::set_new_state(State *new_state) {
    if (new_state != this->state.get()) {
        this->state.reset(new_state);
    }
}
