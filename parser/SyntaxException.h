
#pragma once

#include <exception>
#include <sstream>

#include "../common/token-enum.h"

class SyntaxException : public std::exception {
public:
    SyntaxException(TokenType gottenToken, TokenType expectedToken, int lineNumber = -1)
    {
        std::stringstream ss;
        ss << "Syntax error: expected token '" << TOKEN_NAMES[expectedToken] << "', but got '" << TOKEN_NAMES[gottenToken] << "' on line " << lineNumber << "!";
        message = ss.str();
    };

    SyntaxException(std::string message) : message{message} {}

    const char* what() const throw() {
        return message.c_str();
    }

private:
    std::string message;
};