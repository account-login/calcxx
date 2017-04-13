#include <iostream>
#include <utility>
#include <vector>
#include "catch.hpp"

#include "../tokens.h"
#include "../tokenizer.h"


using namespace std;


vector<Token::Ptr> get_tokens(const string &str) {
    Tokenizer tokenizer;
    for (char ch : str) {
        tokenizer.feed(ch);
    }
    tokenizer.feed('\0');

    vector<Token::Ptr> ans;
    for (Token::Ptr tok = tokenizer.pop(); tok; tok = tokenizer.pop()) {
        ans.push_back(tok);
    }

    CHECK(ans.back()->type == TokenType::END);
    ans.pop_back();
    return ans;
}


TEST_CASE("Test Tokenizer basic") {
    vector<Token::Ptr> tokens;
    tokens = get_tokens("+");
    CHECK(tokens.size() == 1);
    CHECK(tokens[0]->type == TokenType::PLUS);

    tokens = get_tokens("++");
    CHECK(tokens.size() == 2);
    CHECK(tokens[0]->type == TokenType::PLUS);
    CHECK(tokens[1]->type == TokenType::PLUS);

    for (string s : {"+123+", " + 123  + "}) {
        tokens = get_tokens(s);
        REQUIRE(tokens.size() == 3);
        CHECK(tokens[0]->type == TokenType::PLUS);
        CHECK(tokens[1]->type == TokenType::INT);
        CHECK(static_pointer_cast<TokenInt>(tokens[1])->value == 123);
        CHECK(tokens[2]->type == TokenType::PLUS);
    }
}


void check_tokens_pos(const string &str, const vector<pair<SourcePos, SourcePos>> &positions) {
    vector<Token::Ptr> tokens = get_tokens(str);
    REQUIRE(tokens.size() == positions.size());
    for (size_t i = 0; i < tokens.size(); i++) {
        CHECK(tokens[i]->start == positions[i].first);
        CHECK(tokens[i]->end == positions[i].second);
    }
}


TEST_CASE("Test Tokenizer source position") {
    vector<Token::Ptr> tokens;

    check_tokens_pos("+", {{{0, 0}, {0, 0}}});
    check_tokens_pos("++", {
        {{0, 0}, {0, 0}},
        {{0, 1}, {0, 1}}
    });
    check_tokens_pos("+123+", {
        {{0, 0}, {0, 0}},
        {{0, 1}, {0, 3}},
        {{0, 4}, {0, 4}}
    });
    check_tokens_pos("  + \n+ +", {
        {{0, 2}, {0, 2}},
        {{1, 0}, {1, 0}},
        {{1, 2}, {1, 2}}
    });
}


TEST_CASE("Test Tokenizer single char token") {
    string str = "+-*/()";
    vector<Token::Ptr> tokens = get_tokens(str);
    REQUIRE(tokens.size() == str.size());
    for (size_t i = 0; i < tokens.size(); i++) {
        CHECK(tokens[i]->type == static_cast<TokenType>(str[i]));
    }
}


TEST_CASE("Test Tokenizer throw") {
    Tokenizer tokenizer;
    CHECK_THROWS_AS(tokenizer.feed('x'), TokenizerError);
}


TokenInt I(int64_t value) {
    return TokenInt(value);
}


TokenFloat F(double value) {
    return TokenFloat(value);
}


TEST_CASE("Test Tokenizer number") {
    CHECK(*get_tokens("123")[0] == I(123));
    CHECK(*get_tokens("1.2")[0] == F(1.2));
    CHECK(*get_tokens(".2")[0] == F(.2));
    CHECK(*get_tokens(".25")[0] == F(.25));
    CHECK(*get_tokens("2.")[0] == F(2.0));
    CHECK(*get_tokens("1e5")[0] == I(100000));
    CHECK(*get_tokens("1e+5")[0] == I(100000));
    CHECK(*get_tokens("1e50")[0] == F(1e50));
    CHECK(*get_tokens("1.e5")[0] == F(1e5));
    CHECK(*get_tokens("1e-1")[0] == F(1e-1));

    CHECK_THROWS_AS(get_tokens("."), TokenizerError);
    CHECK_THROWS_AS(get_tokens("1.2."), TokenizerError);
    CHECK_THROWS_AS(get_tokens(".e5"), TokenizerError);
    CHECK_THROWS_AS(get_tokens(".2."), TokenizerError);
    CHECK_THROWS_AS(get_tokens("1e+"), TokenizerError);
    CHECK_THROWS_AS(get_tokens("1e"), TokenizerError);
}
