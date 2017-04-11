#include "node.h"


bool Node::operator==(const Node &other) const {
    if (*(this->token) != *other.token) {
        return false;
    }
    if (this->children.size() != other.children.size()) {
        return false;
    }
    for (
        auto it1 = this->children.begin(), it2 = other.children.begin();
        it1 != this->children.end();
        ++it1, ++it2)
    {
        if (**it1 != **it2) {
            return false;
        }
    }
    return true;
}

bool Node::operator!=(const Node &other) const {
    return !(*this == other);
}


string repr_node(const Node &node, unsigned int indent) {
    string ans;
    string pad = string(indent * 4, ' ');
    ans += pad + node.token->_token_name() + " " + node.token->_repr_value() + "\n";
    for (Node::Ptr child : node.children) {
        ans += repr_node(*child, indent + 1);
    }
    return ans;
}
