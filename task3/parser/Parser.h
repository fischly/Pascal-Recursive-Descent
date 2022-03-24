
#include <iostream>

#include "../common/token-enum.h"
#include "../lexer/lex.yy.c"

#include "SyntaxException.h"
#include "AST/Expression.h"

extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE *yyin;

class Parser {
public:
    Parser() {
        nextToken = static_cast<TokenType>(yylex());
    }


// private:
    TokenType nextToken;

    Token match(TokenType expectedToken) {
        if (nextToken == expectedToken) {
            // consume next token
            return match();
        } else {
            throw SyntaxException(nextToken, expectedToken, yylineno);
        }

    }

    /* Matches every token and consumes it */
    Token match() {
        Token consumedToken(nextToken, yytext, yylineno);
        
        std::cout << "updated next token from " << TOKEN_NAMES[nextToken] << " (\"" << yytext << "\") to ";
        nextToken = static_cast<TokenType>(yylex());
        std::cout << TOKEN_NAMES[nextToken] << " (\"" << yytext << "\")" << std::endl;

        return consumedToken;
    }


    Expression::Expression* expression() {
        Expression::Expression* temp = term(); // left expression

        while (nextToken == TokenType::OP_ADD || nextToken == TokenType::OP_SUB) { // TODO: change to all strich operators
            Token operatorToken = match();

            temp = new Expression::Binary(temp, operatorToken, term());
        }

        return temp;
    }

    void simple_expression() {

    }

    Expression::Expression* term() {
        Expression::Expression* temp = factor();

        while (nextToken == TokenType::OP_MUL) { // TODO: change to all punkt operators
            Token operatorToken = match();

            temp = new Expression::Binary(temp, operatorToken, factor());
        }

        return temp;
    }

    Expression::Expression* factor() {
        Expression::Expression* temp;

        switch (nextToken) {
            case TokenType::BRACKETS_OPEN:
                std::cout << "[factor] found BRACKETS_OPEN" << std::endl;

                match(TokenType::BRACKETS_OPEN);
                temp = new Expression::Grouping(expression());
                match(TokenType::BRACKETS_CLOSING);

                break;
            case TokenType::LITERAL_INTEGER:
                std::cout << "[factor] found LITERAL_INTEGER (" << yytext << ")" << std::endl;

                Token literalIntegerToken = match();
                temp = new Expression::Literal{literalIntegerToken};
                // match(TokenType::LITERAL_INTEGER);
                break;
        }

        return temp;
    }

};