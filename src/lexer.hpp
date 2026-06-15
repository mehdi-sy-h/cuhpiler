#ifndef CUH_LEXER_HPP
#define CUH_LEXER_HPP

#include <string>
#include <variant>

namespace Token {
struct EndOfFile {};
struct Definition {};
struct Extern {};
struct Identifier {
    std::string name;
};
struct Number {
    double num;
};
struct Unknown {
    int c;
};

using Token =
    std::variant<EndOfFile, Definition, Extern, Identifier, Number, Unknown>;
} // namespace Token

auto getToken() -> const Token::Token;
auto getNextToken() -> Token::Token;

#endif
