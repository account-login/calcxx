#include <memory>
#include "catch.hpp"
#include "tokens.h"
#include "tokenizer.h"
#include "eval.h"


using std::make_shared;
using namespace Catch::Matchers;


TEST_CASE("Test extract argument") {
    stack<Token::Ptr> input;
    vector<Token::Ptr> output;

    CHECK(extract_argument(input, {}).size() == 0);
    CHECK_THROWS_WITH(extract_argument(input, {"i"}), Contains("missing"));

    input.push(make_shared<TokenInt>(1));
    input.push(make_shared<TokenFloat>(2.0));
    output = extract_argument(input, {"f", "i"});
    REQUIRE(output.size() == 2);
    REQUIRE(input.size() == 0);
    CHECK(*output[0] == TokenInt(1));
    CHECK(*output[1] == TokenFloat(2.0));

    input.push(make_shared<TokenInt>(1));
    input.push(make_shared<TokenInt>(2));
    CHECK_THROWS_WITH(extract_argument(input, {"if", "f"}), Contains("mismatch"));
}


void check_stack_result(stack<Token::Ptr> &stack, const Token &expect) {
    CHECK(stack.size() == 1);
    auto tok = stack.top();
    stack.pop();
    CHECK(*tok == expect);
}


TEST_CASE("Test operator_add") {
    stack<Token::Ptr> stk({make_shared<TokenInt>(1), make_shared<TokenInt>(2)});
    operator_add(stk);
    check_stack_result(stk, TokenInt(3));

    stk.push(make_shared<TokenInt>(1));
    stk.push(make_shared<TokenFloat>(2.0));
    operator_add(stk);
    check_stack_result(stk, TokenFloat(3.0));
}


TEST_CASE("Test operator_div") {
    stack<Token::Ptr> stk({make_shared<TokenInt>(3), make_shared<TokenInt>(2)});
    operator_div(stk);
    check_stack_result(stk, TokenFloat(1.5));

    stk.push(make_shared<TokenInt>(4));
    stk.push(make_shared<TokenInt>(2));
    operator_div(stk);
    check_stack_result(stk, TokenInt(2));

    stk.push(make_shared<TokenInt>(2));
    stk.push(make_shared<TokenInt>(0));
    operator_div(stk);
    check_stack_result(stk, TokenFloat(std::numeric_limits<double>::infinity()));
}


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
    return ans;
}


Token::Ptr eval_string_token_by_token(const string &input) {
    auto tokenizer = Tokenizer();
    auto calc = TokensEvaluator();
    for (size_t i = 0; i <= input.size(); i++) {    // including \0
        tokenizer.feed(input[i]);
        Token::Ptr tok;
        while ((tok = tokenizer.pop())) {
            calc.feed(tok);
        }
    }
    return calc.get_result();
}


TEST_CASE("Test TokensEvalator") {
    CHECK(*eval_string_token_by_token("1 + 1") == TokenInt(2));
    CHECK(*eval_string_token_by_token("1 + 1 + 2") == TokenInt(4));
    CHECK(*eval_string_token_by_token("1 + 2*3") == TokenInt(7));
    CHECK(*eval_string_token_by_token("2*2 + 3") == TokenInt(7));
    CHECK(*eval_string_token_by_token("2") == TokenInt(2));
    CHECK(*eval_string_token_by_token("(2)") == TokenInt(2));
    CHECK(*eval_string_token_by_token("(3 + 4)") == TokenInt(7));
    CHECK(*eval_string_token_by_token("2 * (3 + 4)") == TokenInt(14));
    CHECK(*eval_string_token_by_token("(3 + 4) * 2") == TokenInt(14));
    CHECK(*eval_string_token_by_token("(3 + ((3 + 4 / 2) - 1)) * 2") == TokenInt(14));
    CHECK(*eval_string_token_by_token("((((((2))))))") == TokenInt(2));
}
