#ifndef CALCXX_PARSER_H
#define CALCXX_PARSER_H


#include <vector>
#include "exception.h"
#include "node.h"
#include "tokens.h"


using std::vector;


class ParserError : public BaseException {
public:
    ParserError(const string &msg) : BaseException(msg) {};
};



/*
 * exp  -> ± xexp ± nexp |
 *         ± xexp |
 *         nexp
 * nexp -> xexp ± nexp |
 *         xexp
 * xexp -> lexp * xexp |
 *         lexp / xexp |
 *         lexp
 * lexp -> ( exp ) | number
 */

enum class ParserState {
    exp,
    exp_signed,
    exp_end,
    nexp,
    nexp_end,
    xexp,
    xexp_end,
    lexp,
    lexp_rpar,
};


class Parser {
public:
    Parser() {
        this->enter_exp();
    }
    void feed(Token::Ptr tok);
    Node::Ptr get_result();

private:
    vector<Node::Ptr> nodes;
    vector<ParserState> states;

    void mismatch(vector<TokenType> expects, Token::Ptr got);
    void enter_exp();
    void enter_nexp();
    void enter_xexp();
    void enter_lexp();
    void grow_body();
    void grow_head(Node::Ptr head);
};


#endif //CALCXX_PARSER_H
