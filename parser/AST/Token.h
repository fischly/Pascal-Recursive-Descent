
#pragma once

#include <string.h>

#include "../../common/token-enum.h"

class Token {
public:
    TokenType type;
    char* lexeme;
    int lineNumber;

    Token(TokenType type, char* lexeme, int lineNumber)
        : type{type}, lineNumber{lineNumber}
    {
        this->lexeme = strdup(lexeme); // TODO: free memory
    }
};