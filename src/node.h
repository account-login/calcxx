#ifndef CALCXX_NODE_H
#define CALCXX_NODE_H


#include <memory>
#include <string>
#include <vector>

#include "tokens.h"
#include "utils.hpp"


using std::shared_ptr;
using std::string;
using std::vector;


struct Node {
    typedef shared_ptr<Node> Ptr;
    typedef vector<Ptr> Container;

    Token::Ptr token;
    Container children;

    Node(Token::Ptr tok) : token(tok) {}
    bool operator==(const Node &other) const;
    bool operator!=(const Node &other) const;
};


string repr_node(const Node &node, unsigned int indent = 0);


REPR(Node) {
    return repr_node(value);
}


#endif //CALCXX_NODE_H
