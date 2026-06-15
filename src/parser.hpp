#ifndef CUH_PARSER_HPP
#define CUH_PARSER_HPP

#include <memory>
#include <string>
#include <variant>
#include <vector>

template <class... Ts>
struct overloads : Ts... {
    using Ts::operator()...;
};

namespace AST {

struct Expr;

struct NumberExpr {
    double val;
};

struct VariableExpr {
    std::string name;
};

struct BinaryOpExpr {
    std::unique_ptr<Expr> lhs;
    std::unique_ptr<Expr> rhs;
    char op;
};

struct CallExpr {
    std::string callee;
    std::vector<std::unique_ptr<Expr>> args;
};

using ExprVariant =
    std::variant<NumberExpr, VariableExpr, BinaryOpExpr, CallExpr>;

struct Expr : ExprVariant {
    using ExprVariant::variant;
    // ExprVariant v;
    // explicit Expr(ExprVariant e) : v(std::move(e)) {}
};

struct Prototype {
    std::string name;
    std::vector<std::string> args;
};

struct Function {
    std::unique_ptr<Prototype> proto;
    std::unique_ptr<Expr> body;
};

}; // namespace AST

auto parseDefinition() -> std::unique_ptr<AST::Function>;
auto parseExtern() -> std::unique_ptr<AST::Prototype>;
auto parseTopLevelExpr() -> std::unique_ptr<AST::Function>;

#endif
