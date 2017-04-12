#include <cassert>
#include <memory>
#include <string>
#include "parser.h"


using std::make_shared;
using std::string;


void Parser::feed(Token::Ptr tok) {
    if (this->states.empty()) {
        if (tok->type == TokenType::END) {
            assert(this->nodes.size() == 1);
        } else {
            this->mismatch({TokenType::END}, tok);
        }
    } else if (this->states.back() == ParserState::exp) {
        if (tok->type == TokenType::PLUS || tok->type == TokenType::MINUS) {
            Node::Ptr node = make_shared<Node>(tok);
            this->nodes.push_back(node);
            this->states.back() = ParserState::exp_signed;
            this->enter_xexp();
        } else {
            this->states.back() = ParserState::exp_cont;
            this->enter_xexp();
            this->feed(tok);
        }
    } else if (this->states.back() == ParserState::exp_signed) {
        this->grow_body();
        this->states.back() = ParserState::exp_cont;
        this->feed(tok);
    } else if(this->states.back() == ParserState::exp_cont) {
        if (tok->type == TokenType::PLUS || tok->type == TokenType::MINUS) {
            Node::Ptr node = make_shared<Node>(tok);
            this->grow_head(node);
            this->states.back() = ParserState::exp_end;
            this->enter_xexp();
        } else {
            this->states.pop_back();
            this->feed(tok);
        }
    } else if (this->states.back() == ParserState::exp_end) {
        this->grow_body();
        this->states.back() = ParserState::exp_cont;
        this->feed(tok);
    } else if (this->states.back() == ParserState::xexp) {
        this->states.back() = ParserState::xexp_cont;
        this->feed(tok);
    } else if (this->states.back() == ParserState::xexp_cont) {
        if (tok->type == TokenType::MULT || tok->type == TokenType::DIV) {
            Node::Ptr node = make_shared<Node>(tok);
            this->grow_head(node);
            this->states.back() = ParserState::xexp_end;
            this->enter_lexp();
        } else {
            this->states.pop_back();
            this->feed(tok);
        }
    } else if (this->states.back() == ParserState::xexp_end) {
        this->grow_body();
        this->states.back() = ParserState::xexp_cont;
        this->feed(tok);
    } else if (this->states.back() == ParserState::lexp) {
        if (tok->type == TokenType::LPAR) {
            this->states.back() = ParserState::lexp_rpar;
            this->enter_exp();
        } else if (tok->type == TokenType::INT || tok->type == TokenType::FLOAT) {
            Node::Ptr node = make_shared<Node>(tok);
            this->nodes.push_back(node);
            this->states.pop_back();
        } else {
            this->mismatch({TokenType::LPAR, TokenType::INT, TokenType::FLOAT}, tok);
        }
    } else if (this->states.back() == ParserState::lexp_rpar) {
        if (tok->type == TokenType::RPAR) {
            this->states.pop_back();
        } else {
            this->mismatch({TokenType::RPAR}, tok);
        }
    } else {
        assert(!"Unreachable");
    }
}

void Parser::mismatch(vector<TokenType> expects, Token::Ptr got) {
    string expected_types;
    expected_types.reserve(expects.size());
    for (TokenType tt : expects) {
        expected_types.push_back((char)tt);
    }
    string msg = "expected token types: expect '" + expected_types + "'"
        + " got " + got->_repr_short() + "\n";
    throw ParserError(msg);
}

void Parser::enter_exp() {
    this->states.push_back(ParserState::exp);
}

void Parser::enter_xexp() {
    this->states.push_back(ParserState::xexp);
    this->enter_lexp();
}

void Parser::enter_lexp() {
    this->states.push_back(ParserState::lexp);
}

Node::Ptr Parser::get_result() {
    assert(this->states.empty() && this->nodes.size() == 1);
    Node::Ptr node = this->nodes.back();
    this->nodes.pop_back();
    return node;
}

void Parser::grow_body() {
    Node::Ptr child = this->nodes.back();
    this->nodes.pop_back();
    this->nodes.back()->children.push_back(child);
}

void Parser::grow_head(Node::Ptr head) {
    Node::Ptr child = this->nodes.back();
    head->children.push_back(child);
    this->nodes.back() = head;
}
