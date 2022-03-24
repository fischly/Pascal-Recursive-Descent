
#include <iostream>

#include "../common/token-enum.h"
#include "../lexer/lex.yy.c"

#include "SyntaxException.h"
#include "AST/Expression.h"

extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE *yyin;

using Expr::Expression;

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


    Expression* expression() {
        Expression* temp = term(); // left expression

        while (nextToken == TokenType::OP_ADD || nextToken == TokenType::OP_SUB) { // TODO: change to all strich operators
            Token operatorToken = match();

            temp = new Expr::Binary(temp, operatorToken, term());
        }

        return temp;
    }

    void simple_expression() {

    }

    Expression* term() {
        Expression* temp = factor();

        while (nextToken == TokenType::OP_MUL) { // TODO: change to all punkt operators
            Token operatorToken = match();

            temp = new Expr::Binary(temp, operatorToken, factor());
        }

        return temp;
    }

    Expression* factor() {
        Expression* temp;

        switch (nextToken) {
            case TokenType::BRACKETS_OPEN:
            {
                std::cout << "[factor] found BRACKETS_OPEN" << std::endl;

                match(TokenType::BRACKETS_OPEN);
                temp = new Expr::Grouping(expression());
                match(TokenType::BRACKETS_CLOSING);

            } break;

            case TokenType::LITERAL_INTEGER:
            case TokenType::LITERAL_REAL:
            case TokenType::LITERAL_STRING: 
            {
                std::cout << "[factor] found LITERAL (" << yytext << ")" << std::endl;

                Token literalToken = match();
                temp = new Expr::Literal{literalToken};
            } break;

            case TokenType::IDENTIFIER:
            {
                Token identifierToken = match();

                // check if we have a function call (starts with opening bracket)
                if (nextToken == TokenType::BRACKETS_OPEN) {
                    // a function call
                    match(TokenType::BRACKETS_OPEN);

                    std::cout << "[factor] found FUNCTION CALL (" << yytext << ")" << std::endl;
                    temp = new Expr::Call(identifierToken, std::vector<Expression*>{}); // TODO: parse arguments as expression list

                    match(TokenType::BRACKETS_CLOSING);
                    
                } else {
                    // just a regular identifier
                    std::cout << "[factor] found IDENTIFIER (" << yytext << ")" << std::endl;
                    temp = new Expr::Identifier(identifierToken);
                }
            } break;
        }

        return temp;
    }

};