#ifndef CALCXX_PARSER_H
#define CALCXX_PARSER_H


#include <vector>

#include "exception.h"
#include "node.h"
#include "tokens.h"


using std::vector;



/*
 * exp  -> ± xexp exp_cont
 *       |   xexp exp_cont
 * exp_cont -> [± xexp]*
 * xexp -> lexp xexp_cont
 * xpex_cont -> [* lexp]*
 * lexp -> ( exp ) | number
 */

enum class ParserState {
    exp,
    exp_cont,
    exp_signed,
    exp_end,
    xexp,
    xexp_cont,
    xexp_end,
    lexp,
    lexp_rpar,
};


class Parser {
public:
    Parser() {
        this->enter_exp();
    }
    void feed(const Token::Ptr &tok);
    Node::Ptr get_result();

private:
    vector<Node::Ptr> nodes;
    vector<ParserState> states;

    void mismatch(vector<TokenType> expects, const Token::Ptr &got);
    void enter_exp();
    void enter_xexp();
    void enter_lexp();
    void grow_body();
    void grow_head(Node::Ptr head);
};


#endif //CALCXX_PARSER_H
