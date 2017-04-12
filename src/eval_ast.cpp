#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include "eval_ast.h"
#include "exception.h"
#include "operators.h"
#include "tokens.h"


using std::back_inserter;
using std::string;
using std::transform;
using std::vector;


static bool is_value_type(Token::Ptr tok) {
    return tok->type == TokenType::INT || tok->type == TokenType::FLOAT;
}


Token::Ptr eval_node(Node::Ptr node) {
    if (is_value_type(node->token)) {
        return node->token;
    }

    TokenType tt = node->token->type;
    auto it = g_builtin_operator_table.find(tt);
    if (it != g_builtin_operator_table.end()) {
        auto func = it->second;
        vector<Token::Ptr> args;
        const Node::Container &children = node->children;
        transform(children.begin(), children.end(), back_inserter(args), eval_node);
        return func(args);
    } else {
        throw NotImplementedOperation(string(1, static_cast<char>(tt)));
    }
}
