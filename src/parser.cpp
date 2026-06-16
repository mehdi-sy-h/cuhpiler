#include "parser.hpp"
#include "lexer.hpp"

#include <iostream>
#include <map>
#include <memory>

extern std::string identifierStr;
extern double numVal;
extern Token::Token curTok;

auto logError(const std::string &errorStr) -> std::unique_ptr<AST::Expr> {
    std::cerr << errorStr << '\n';
    return nullptr;
}

auto logErrorProto(const std::string &errorStr)
    -> std::unique_ptr<AST::Prototype> {
    logError(errorStr);
    return nullptr;
}

// expression
//   ::= primary binoprhs
static auto parseExpression() -> std::unique_ptr<AST::Expr>;

// numberexpr ::= number
static auto parseNumberExpr() -> std::unique_ptr<AST::Expr> {
    auto result = std::make_unique<AST::Expr>(AST::NumberExpr {numVal});
    getNextToken();
    return result;
}

// parenexpr ::= '(' expression ')'
static auto parseParenExpr() -> std::unique_ptr<AST::Expr> {
    getNextToken(); // eat (
    auto v = parseExpression();
    if (!v)
        return nullptr;

    if (!std::get_if<Token::RParen>(&curTok))
        return logError("expected ')'");

    getNextToken(); // eat )

    return v;
}

// identifierexpr
//   ::= identifier
//   ::= identifier '(' expression* ')'
static auto parseIdentifierExpr() -> std::unique_ptr<AST::Expr> {
    const std::string idName = identifierStr;

    getNextToken(); // eat identifier

    if (!std::get_if<Token::LParen>(&curTok))
        return std::make_unique<AST::Expr>(AST::VariableExpr {idName});

    getNextToken(); // eat (

    std::vector<std::unique_ptr<AST::Expr>> args;

    if (!std::get_if<Token::RParen>(&curTok)) {
        while (true) {
            if (auto arg = parseExpression()) {
                args.push_back(std::move(arg));
            } else {
                return nullptr;
            }

            if (std::get_if<Token::RParen>(&curTok))
                break;

            if (!std::get_if<Token::Comma>(&curTok))
                return logError("Expected ')' or ',' in argument list");

            getNextToken();
        }
    }

    getNextToken(); // eat )

    return std::make_unique<AST::Expr>(AST::CallExpr {idName, std::move(args)});
}

static auto parsePrimary() -> std::unique_ptr<AST::Expr> {
    return curTok.visit(
        overloads {
            [](const Token::Identifier &) {
                return parseIdentifierExpr();
            },
            [](const Token::Number &) {
                return parseNumberExpr();
            },
            [](const Token::LParen &) {
                return parseParenExpr();
            },
            [](const auto &) {
                return logError("unknown token when expecting an expression");
            }
        }
    );
}

static const std::map<char, int> binopPrecedence {
    {'<', 10},
    {'+', 20},
    {'-', 20},
    {'*', 40},
};

static auto getTokPrecedence() -> int {
    auto *unknown = std::get_if<Token::Unknown>(&curTok);
    if (unknown && binopPrecedence.contains(unknown->c)) {
        return binopPrecedence.at(unknown->c);
    }
    return -1;
}

// binoprhs
//   ::= ('+' primary)*
static auto parseBinOpRHS(int minExprPrec, std::unique_ptr<AST::Expr> lhs)
    -> std::unique_ptr<AST::Expr> {
    while (true) {
        int tokPrec = getTokPrecedence();

        if (tokPrec < minExprPrec)
            return lhs;

        auto binOp = std::get<Token::Unknown>(curTok);
        getNextToken();

        auto rhs = parsePrimary();
        if (!rhs)
            return nullptr;

        int nextPrec = getTokPrecedence();
        if (tokPrec < nextPrec) {
            rhs = parseBinOpRHS(tokPrec + 1, std::move(rhs));
            if (!rhs)
                return nullptr;
        }

        lhs = std::make_unique<AST::Expr>(AST::BinaryOpExpr {
            std::move(lhs), std::move(rhs), static_cast<char>(binOp.c)
        });
    }
}

// expression
//   ::= primary binoprhs
static auto parseExpression() -> std::unique_ptr<AST::Expr> {
    auto lhs = parsePrimary();
    if (!lhs)
        return nullptr;

    return parseBinOpRHS(0, std::move(lhs));
}

// prototype
//   ::= id '( id* ')'
static auto parsePrototype() -> std::unique_ptr<AST::Prototype> {
    if (!std::get_if<Token::Identifier>(&curTok))
        return logErrorProto("Expected function name in prototype");

    std::string fnName = identifierStr;
    getNextToken();

    if (!std::get_if<Token::LParen>(&curTok))
        return logErrorProto("Expected '(' in prototype");

    std::vector<std::string> argNames;

    auto argsTok = getNextToken();
    while (std::get_if<Token::Identifier>(&argsTok)) {
        argNames.push_back(identifierStr);
        argsTok = getNextToken();
    }

    if (!std::get_if<Token::RParen>(&curTok))
        return logErrorProto("Expected ')' in prototype");

    getNextToken();

    return std::make_unique<AST::Prototype>(
        AST::Prototype {fnName, std::move(argNames)}
    );
}

// definition
//   ::= 'def' prototype expression
auto parseDefinition() -> std::unique_ptr<AST::Function> {
    getNextToken();

    auto proto = parsePrototype();
    if (!proto)
        return nullptr;

    if (auto e = parseExpression())
        return std::make_unique<AST::Function>(std::move(proto), std::move(e));

    return nullptr;
}

// external
//   ::= 'extern' prototype
auto parseExtern() -> std::unique_ptr<AST::Prototype> {
    getNextToken();
    return parsePrototype();
}

// toplevelexpr
//   ::= expression
auto parseTopLevelExpr() -> std::unique_ptr<AST::Function> {
    if (auto e = parseExpression()) {
        auto proto = std::make_unique<AST::Prototype>(
            "__anon_expr", std::vector<std::string> {}
        );
        return std::make_unique<AST::Function>(std::move(proto), std::move(e));
    }
    return nullptr;
}
