#include <memory>
#include "tokenizer.h"


using std::make_shared;


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
        return make_tuple(Token::Ptr(), true, this);
    }
    SINGLE_CHAR('+', PLUS)
    SINGLE_CHAR('-', MINUS)
    SINGLE_CHAR('*', MULT)
    SINGLE_CHAR('/', DIV)
    SINGLE_CHAR('(', LPAR)
    SINGLE_CHAR(')', RPAR)
    else if (isdigit(ch)) {
        return make_tuple(Token::Ptr(), false, new NumberState);
    } else {
        throw TokenizerError();
    }

#undef SINGLE_CHAR
}


tuple<Token::Ptr, bool, State*> NumberState::feed(char ch) {
    if (isdigit(ch)) {
        this->digits.push_back(ch);
        return make_tuple(Token::Ptr(), true, this);
    } else {
        int64_t value = atoll(this->digits.data());
        Token::Ptr tok = make_shared<TokenInt>(value);
        return make_tuple(tok, false, new InitState);
    }
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
