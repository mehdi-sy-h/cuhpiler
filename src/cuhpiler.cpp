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
        bool quit = curTok.visit(
            overloads {
                [](const Token::EndOfFile &) {
                    return true;
                },
                [](const Token::Definition &) {
                    handleDefinition();
                    return false;
                },
                [](const Token::Extern &) {
                    handleExtern();
                    return false;
                },
                [](const Token::Unknown &tok) {
                    if (tok.c == ';') {
                        getNextToken();
                    } else {
                        handleTopLevelExpr();
                    }
                    return false;
                },
                [](const auto &) {
                    handleTopLevelExpr();
                    return false;
                }
            }
        );
        if (quit)
            return;
    }
}

// Following along with LLVM Kaleidescope tutorial
auto main() -> int {
    std::cerr << "ready> ";
    getNextToken();

    mainLoop();

    return 0;
}
