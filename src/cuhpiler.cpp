#include <iostream>

// Following along with LLVM Kaleidescope tutorial
static std::string IdentifierStr;
static double NumVal;

enum class Token {
    END_OF_FILE = -1,
    DEFINITION = -2,
    EXTERN = -3,
    IDENTIFIER = -4,
    NUMBER = -5,
};

static auto getToken() -> Token {
    static char LastChar {' '};

    while (std::isspace(LastChar))
        std::cin.get(LastChar);

    if (std::isalnum(LastChar)) {
        IdentifierStr = LastChar;
        while (std::cin.get(LastChar) && std::isalnum(LastChar))
            IdentifierStr += LastChar;

        if (IdentifierStr == "def")
            return Token::DEFINITION;

        if (IdentifierStr == "extern")
            return Token::EXTERN;

        return Token::IDENTIFIER;
    }

    if (std::isdigit(LastChar) || LastChar == '.') {
        std::string NumStr {LastChar};
        while (std::cin.get(LastChar) &&
               (std::isdigit(LastChar) || LastChar == '.'))
            NumStr += LastChar;

        NumVal = std::strtod(NumStr.c_str(), 0);
        return Token::NUMBER;
    }

    if (LastChar == '#') {
        do
            std::cin.get(LastChar);
        while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

        if (LastChar != EOF)
            return getToken();
    }

    if (LastChar == EOF)
        return Token::END_OF_FILE;

    int thisChar {LastChar};
    std::cin.get(LastChar);
    return Token {thisChar};
}

auto main() -> int {}
