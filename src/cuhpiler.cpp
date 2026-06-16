#include "lexer.hpp"
#include "parser.hpp"

#include <iostream>

std::string identifierStr;
double numVal;
Token::Token curTok;

static auto handleTopLevelExpr() {
    if (parseTopLevelExpr()) {
        std::cerr << "Parsed a top level expression\n";
    } else {
        getNextToken();
    }
}

static auto handleExtern() {
    if (parseExtern()) {
        std::cerr << "Parsed an extern\n";
    } else {
        getNextToken();
    }
}

static auto handleDefinition() {
    if (parseDefinition()) {
        std::cerr << "Parsed a function definition\n";
    } else {
        getNextToken();
    }
}

// top
//   ::= definition | external | expression | ';'
static auto mainLoop() {
    while (true) {
        std::cerr << "ready> ";

        if (std::get_if<Token::EndOfFile>(&curTok))
            return;

        curTok.visit(
            overloads {
                [](const Token::Definition &) {
                    handleDefinition();
                },
                [](const Token::Extern &) {
                    handleExtern();
                },
                [](const Token::Semi &) {
                    getNextToken();
                },
                [](const auto &) {
                    handleTopLevelExpr();
                }
            }
        );
    }
}

// Following along with LLVM Kaleidescope tutorial
auto main() -> int {
    std::cerr << "ready> ";
    getNextToken();

    mainLoop();

    return 0;
}
