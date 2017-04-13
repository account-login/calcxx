#include <cassert>
#include <exception>
#include <iostream>
#include <string>

#include "eval.h"
#include "eval_ast.h"
#include "exception.h"
#include "node.h"
#include "parser.h"
#include "sourcepos.h"
#include "tokenizer.h"
#include "tokens.h"


using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::exception;
using std::getline;
using std::string;
using std::to_string;


static void report_error(
    const string &msg,
    const SourcePos &start, const SourcePos &end, size_t prompt_len = 0)
{
    int len = end.rowno - start.rowno;
    assert(len >= 0);
    size_t padding = (size_t)start.rowno + prompt_len;
    cerr << string(padding, ' ') << "^" << string((size_t)len, '~') << endl;
    cerr << msg << endl;
}


class AstEvaluator {
public:
    AstEvaluator() {}

    void feed(const Token::Ptr &tok) {
        this->parser.feed(tok);
    }

    Token::Ptr get_result() {
        Node::Ptr ast = this->parser.get_result();
        return eval_node(ast);
    }

    void reset() {
        this->parser = Parser();
    }

private:
    Parser parser;
};


template<class EvaluatorType>
void main_func() {
    Tokenizer tokenizer;
    EvaluatorType evaluator;

    for (int count = 0; !cin.eof(); count++) {
        string prompt = "[" + to_string(count) + "] ";
        string line;

        cout << prompt;
        getline(cin, line);
        for (size_t i = 0; i <= line.size(); i++) {
            try {
                tokenizer.feed(line[i]);
            } catch (const TokenizerError &exc) {
                // report
                SourcePos pos = SourcePos(0, (int)i);
                const string msg = string("TokenizerError: ") + exc.what();
                report_error(msg, pos, pos, prompt.size());
                goto CLEAN_UP;
            }

            Token::Ptr tok;
            while ((tok = tokenizer.pop())) {
                try {
                    evaluator.feed(tok);
                    if (tok->type == TokenType::END) {
                        Token::Ptr result = evaluator.get_result();
                        cout << result->_repr_value() << endl;
                        // cout << repr(*evaluator.get_result()) << endl;
                    }
                } catch (const EvalError &exc) {
                    const string msg = string("EvalError: ") + exc.what();
                    report_error(msg, tok->start, tok->end, prompt.size());
                    goto CLEAN_UP;
                } catch (const ParserError &exc) {
                    const string msg = string("ParserError: ") + exc.what();
                    report_error(msg, tok->start, tok->end, prompt.size());
                    goto CLEAN_UP;
                }
            }
        }

    CLEAN_UP:
        tokenizer.reset();
        evaluator.reset();
    }
}


int main(int argc, const char *argv[]) {
    string arg = "-p";
    if (argc > 1) {
        arg = string(argv[1]);
    }

    if (arg == "-p") {
        main_func<AstEvaluator>();
    } else {
        main_func<TokensEvaluator>();
    }
    return 0;
}
