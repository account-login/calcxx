#include <memory>
#include "catch.hpp"
#include "operators.h"


using std::make_shared;


Token::Ptr T(int value) {
    return make_shared<TokenInt>(value);
}


Token::Ptr T(double value) {
    return make_shared<TokenFloat>(value);
}


TEST_CASE("Test operator_add") {
    CHECK(*op_add({T(1), T(2)}) == *T(3));
    CHECK(*op_add({T(1), T(2.0)}) == *T(3.0));
}


TEST_CASE("Test operator_div") {
    CHECK(*op_div({T(3), T(2)}) == *T(1.5));
    CHECK(*op_div({T(4), T(2)}) == *T(2));
    CHECK(*op_div({T(2), T(0)}) == *T(std::numeric_limits<double>::infinity()));
}
