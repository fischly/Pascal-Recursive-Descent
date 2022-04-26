
#pragma once

#include <string.h>

enum TokenType {
    EOF_ = 0,

    /* Keywords */
    PROGRAM = 1,
    FUNCTION,
    PROCEDURE,

    BEGIN_,
    END_,

    IF,
    THEN,
    ELSE,
    WHILE,
    DO,

    VAR,
    OF,

    /* Syntactical symbols */
    COMMA,
    COLON,
    SEMICOLON,
    DOT,
    RANGE_DOTS,

    BRACKETS_OPEN,
    BRACKETS_CLOSING,
    SQUARE_OPEN,
    SQUARE_CLOSING,
    
    /* Data types */
    INTEGER,
    REAL,
    BOOLEAN,
    ARRAY,

    /* Operators */
    OP_ASSIGNMENT,

    OP_NOT,

    OP_EQUALS,
    OP_NOT_EQUALS,
    OP_LESS,
    OP_LESS_EQUAL,
    OP_GREATER,
    OP_GREATER_EQUAL,

    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_INTEGER_DIV,

    OP_AND,
    OP_OR,

    /* Literals */
    LITERAL_INTEGER,
    LITERAL_REAL,
    LITERAL_STRING,
    LITERAL_TRUE,
    LITERAL_FALSE,

    /* Identifier */
    IDENTIFIER,

    EPSILON
};


/* store the token names seperated from the enum, since C++ has no means of 
   accessing the name of enum fields */
const char* TOKEN_NAMES[] = {
    [0] = "EOF",

    /* Keywords */
    [TokenType::PROGRAM] = "PROGRAM",
    [TokenType::FUNCTION] = "FUNCTION",
    [TokenType::PROCEDURE] = "PROCEDURE",

    [TokenType::BEGIN_] = "BEGIN",
    [TokenType::END_] = "END",

    [TokenType::IF] = "IF",
    [TokenType::THEN] = "THEN",
    [TokenType::ELSE] = "ELSE",
    [TokenType::WHILE] = "WHILE",
    [TokenType::DO] = "DO",

    [TokenType::VAR] = "VAR",
    [TokenType::OF] = "OF",

    /* Syntactical symbols */
    [TokenType::COMMA] = "COMMA",
    [TokenType::COLON] = "COLON",
    [TokenType::SEMICOLON] = "SEMICOLON",
    [TokenType::DOT] = "DOT",
    [TokenType::RANGE_DOTS] = "RANGE_DOTS",

    [TokenType::BRACKETS_OPEN] = "BRACKETS_OPEN",
    [TokenType::BRACKETS_CLOSING] = "BRACKETS_CLOSING",
    [TokenType::SQUARE_OPEN] = "SQUARE_OPEN",
    [TokenType::SQUARE_CLOSING] = "SQUARE_CLOSING",

    /* Data types */
    [TokenType::INTEGER] = "INTEGER",
    [TokenType::REAL] = "REAL",
    [TokenType::BOOLEAN] = "BOOLEAN",
    [TokenType::ARRAY] = "ARRAY",

    /* Operators */
    [TokenType::OP_ASSIGNMENT] = "OP_ASSIGNMENT",

    [TokenType::OP_NOT] = "OP_NOT",
    
    [TokenType::OP_EQUALS] = "OP_EQUALS",
    [TokenType::OP_NOT_EQUALS] = "OP_NOT_EQUALS",
    [TokenType::OP_LESS] = "OP_LESS",
    [TokenType::OP_LESS_EQUAL] = "OP_LESS_EQUAL",
    [TokenType::OP_GREATER] = "OP_GREATER",
    [TokenType::OP_GREATER_EQUAL] = "OP_GREATER_EQUAL",

    [TokenType::OP_ADD] = "OP_ADD",
    [TokenType::OP_SUB] = "OP_SUB",
    [TokenType::OP_MUL] = "OP_MUL",
    [TokenType::OP_DIV] = "OP_DIV",
    [TokenType::OP_INTEGER_DIV] = "OP_INTEGER_DIV",

    [TokenType::OP_AND] = "OP_AND",
    [TokenType::OP_OR] = "OP_OR",

    /* Literals */
    [TokenType::LITERAL_INTEGER] = "LITERAL_INTEGER",
    [TokenType::LITERAL_REAL] = "LITERAL_REAL",
    [TokenType::LITERAL_STRING] = "LITERAL_STRING",

    [TokenType::LITERAL_TRUE] = "LITERAL_TRUE",
    [TokenType::LITERAL_FALSE] = "LITERAL_FALSE",

    /* Identifier */
    [TokenType::IDENTIFIER] = "IDENTIFIER",

    [TokenType::EPSILON] = "NOTHING"

};