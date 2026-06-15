#include "lexer.hpp"

#include <iostream>

extern std::string identifierStr;
extern double numVal;
extern Token::Token curTok;

auto getToken() -> const Token::Token {
    static int lastChar {' '};

    while (std::isspace(lastChar))
        lastChar = std::cin.get();

    if (std::isalpha(lastChar)) {
        identifierStr = lastChar;
        while (std::isalnum((lastChar = std::cin.get())))
            identifierStr += lastChar;

        if (identifierStr == "def")
            return Token::Definition {};

        if (identifierStr == "extern")
            return Token::Extern {};

        return Token::Identifier {identifierStr};
    }

    if (std::isdigit(lastChar) || lastChar == '.') {
        std::string NumStr {static_cast<char>(lastChar)};
        while (std::isdigit(lastChar = std::cin.get()) || lastChar == '.')
            NumStr += lastChar;

        numVal = std::strtod(NumStr.c_str(), nullptr);
        return Token::Number {numVal};
    }

    if (lastChar == '#') {
        do
            lastChar = std::cin.get();
        while (lastChar != EOF && lastChar != '\n' && lastChar != '\r');

        if (lastChar != EOF)
            return getToken();
    }

    if (lastChar == EOF)
        return Token::EndOfFile {};

    int thisChar {lastChar};
    lastChar = std::cin.get();
    return Token::Unknown {(char)thisChar};
}

auto getNextToken() -> Token::Token {
    return curTok = getToken();
}
