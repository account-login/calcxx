#include <memory>
#include <string>
#include "catch.hpp"
#include "../node.h"
#include "../parser.h"
#include "../tokenizer.h"


using std::make_shared;
using std::string;


Node::Ptr parse(const string &str) {
    Tokenizer tokenizer;
    Parser parser;
    Token::Ptr tok;

    for (size_t i = 0; i <= str.size(); i++) {
        tokenizer.feed(str[i]);
    }
    while ((tok = tokenizer.pop())) {
        parser.feed(tok);
    }

    return parser.get_result();
}


Node::Ptr add_children(Node::Ptr node, Node::Ptr c1, Node::Ptr c2) {
    for (Node::Ptr child : {c1, c2}) {
        if (child) {
            node->children.push_back(child);
        }
    }
    return node;
}


Node::Ptr N(char type_char, Node::Ptr c1 = Node::Ptr(), Node::Ptr c2 = Node::Ptr()) {
    Token::Ptr tok = make_shared<Token>(static_cast<TokenType>(type_char));
    Node::Ptr node = make_shared<Node>(tok);
    return add_children(node, c1, c2);
}


Node::Ptr N(int value, Node::Ptr c1 = Node::Ptr(), Node::Ptr c2 = Node::Ptr()) {
    Token::Ptr tok = make_shared<TokenInt>(value);
    Node::Ptr node = make_shared<Node>(tok);
    return add_children(node, c1, c2);
}


TEST_CASE("Test Node comparison") {
    CHECK(*N(1) != *N(2));
    CHECK(*N('+') != *N('+', N(1)));
    CHECK(*N('+', N(2)) != *N('+', N(1)));
}


TEST_CASE("Test parser") {
    CHECK(*parse("1") == *N(1));
    CHECK(*parse("1 + 2") == *N('+', N(1), N(2)));
    CHECK(*parse("(1)") == *parse("1"));
    CHECK(*parse("1 + 2 * 3") == *N(
        '+',
        N(1),
        N('*', N(2), N(3))
    ));
    CHECK(*parse("1 * 2 + 3") == *N(
        '+',
        N('*', N(1), N(2)),
        N(3)
    ));
    CHECK(*parse("(1 + 2) * 3") == *N(
        '*',
        N('+', N(1), N(2)),
        N(3)
    ));
    CHECK(*parse("1 + 2 + 3") == *N(
        '+',
        N('+', N(1), N(2)),
        N(3)
    ));
    CHECK(*parse("1 - 2 + 3") == *N(
        '+',
        N('-', N(1), N(2)),
        N(3)
    ));
    CHECK(*parse("1 * 2 / 3") == *N(
        '/',
        N('*', N(1), N(2)),
        N(3)
    ));
    CHECK(*parse("+1") == *N('+', N(1)));
    CHECK(*parse("-1") == *N('-', N(1)));
    CHECK(*parse("-1 + 2") == *N(
        '+',
        N('-', N(1)),
        N(2)
    ));
    CHECK(*parse("-1 + 2 - 3 + 4") == *N(
        '+',
        N('-',
            N('+',
                N('-', N(1)),
                N(2)),
            N(3)),
        N(4)
    ));
    CHECK(*parse("-1 * 2") == *N(
        '-',
        N('*', N(1), N(2))
    ));
    CHECK(*parse("-1 * 2 + 3") == *N(
        '+',
        N('-',
            N('*', N(1), N(2))),
        N(3)
    ));
}


TEST_CASE("Test parser bad input") {
    CHECK_THROWS_AS(parse(""), ParserError);
    CHECK_THROWS_AS(parse("+"), ParserError);
    CHECK_THROWS_AS(parse("1+"), ParserError);
    CHECK_THROWS_AS(parse("()"), ParserError);
    CHECK_THROWS_AS(parse("(1)+"), ParserError);
    CHECK_THROWS_AS(parse("1 2"), ParserError);
    CHECK_THROWS_AS(parse("*1"), ParserError);
    CHECK_THROWS_AS(parse("(1"), ParserError);
}
