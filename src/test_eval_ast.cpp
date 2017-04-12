#include <string>
#include "catch.hpp"

#include "eval_ast.h"
#include "node.h"
#include "parser.h"
#include "tokenizer.h"
#include "tokens.h"


using std::string;


Token::Ptr eval_string(const string &str) {
    Tokenizer tokenizer;
    for (size_t i = 0; i <= str.size(); i++) {
        tokenizer.feed(str[i]);
    }

    Parser parser;
    Token::Ptr tok;
    while ((tok = tokenizer.pop())) {
        parser.feed(tok);
    }

    Node::Ptr ast = parser.get_result();
    return eval_node(ast);
}


TEST_CASE("Test eval_node") {
    CHECK(*eval_string("1") == TokenInt(1));
    CHECK(*eval_string("1 + 1") == TokenInt(2));
    CHECK(*eval_string("-5 - 1 + 2 * 3") == TokenInt(0));
}
