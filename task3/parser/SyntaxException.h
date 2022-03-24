
#pragma once

#include <exception>
#include <sstream>

#include "../common/token-enum.h"

class SyntaxException : public std::exception {
public:
    SyntaxException(TokenType gottenToken, TokenType expectedToken, int lineNumber = -1)
        : tokenGotten{gottenToken}, tokenExpected{expectedToken}, lineNumber{lineNumber}
    {};

    const char* what() const throw() {
        std::stringstream ss;
        ss << "Syntax error: expected token '" << TOKEN_NAMES[this->tokenExpected] << "', but got '" << TOKEN_NAMES[this->tokenGotten] << "' on line " << lineNumber << "!";
        return ss.str().c_str();
    }

private:
    TokenType tokenGotten;
    TokenType tokenExpected;
    int lineNumber;
};