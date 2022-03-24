
#include <iostream>

#include "../common/token-enum.h"
#include "../lexer/lex.yy.c"

#include "SyntaxException.h"
#include "AST/Expression.h"
#include "AST/Statement.h"

extern int yylex();
extern int yylineno;
extern char* yytext;
extern FILE *yyin;

using Expr::Expression;
using Stmt::Statement;

class Parser {
public:
    Parser() {
        // consume first token at start
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
        
        // std::cout << "updated next token from " << TOKEN_NAMES[nextToken] << " (\"" << yytext << "\") to ";
        nextToken = static_cast<TokenType>(yylex());
        // std::cout << TOKEN_NAMES[nextToken] << " (\"" << yytext << "\")" << std::endl;

        return consumedToken;
    }


    /* =========================================================================================================================== */
    /* ========= Statements  ===================================================================================================== */
    /* =========================================================================================================================== */

    Statement* statement() {
        Statement* temp;

        // block
        if (nextToken == TokenType::BEGIN_) {
            match(TokenType::BEGIN_);

            std::vector<Statement*> statementsInBlock;
            while (nextToken != TokenType::END_) {
                statementsInBlock.push_back(statement());
            }

            temp = new Stmt::Block(statementsInBlock);

            match(TokenType::END_);
        }
        // while
        else if (nextToken == TokenType::WHILE) {
            match(TokenType::WHILE);

            Expression* condition = expression();
            match(TokenType::DO);
            Statement* body = statement();

            temp = new Stmt::While(condition, body);
        }
        // if
        else if (nextToken == TokenType::IF) {
            match(TokenType::IF);

            Expression* condition = expression();
            match(TokenType::THEN);
            Statement* thenBody = statement();
            Statement* elseBody = NULL;

            if (nextToken == TokenType::ELSE) {
                match(TokenType::ELSE);

                elseBody = statement();
            }

            temp = new Stmt::If(condition, thenBody, elseBody);
        }

        else if (nextToken == TokenType::IDENTIFIER) {
            Token identifierToken = match(TokenType::IDENTIFIER);
            
            // method call
            if (nextToken == TokenType::BRACKETS_OPEN) {
                match(TokenType::BRACKETS_OPEN);

                std::vector<Expression*> argumentList;
                if (nextToken != TokenType::BRACKETS_CLOSING) {
                    argumentList.push_back(expression());

                    while (nextToken == TokenType::COMMA) {
                        match(TokenType::COMMA);
                        argumentList.push_back(expression());
                    }
                }

                temp = new Stmt::Call(identifierToken, argumentList); 

                match(TokenType::BRACKETS_CLOSING);
            }
            // assignment
            else {
                Expression* arrayIndexValue = NULL;
                if (nextToken == TokenType::SQUARE_OPEN) {
                    match(TokenType::SQUARE_OPEN);
                    arrayIndexValue = expression();
                    match(TokenType::SQUARE_CLOSING);
                }

                match(TokenType::OP_ASSIGNMENT);

                Expression* assignmentValue = expression();

                temp = new Stmt::Assignment(identifierToken, arrayIndexValue, assignmentValue);
            }
        }

        return temp;
    }



    /* =========================================================================================================================== */
    /* ========= Expressions ===================================================================================================== */
    /* =========================================================================================================================== */

    Expression* expression() {
        Expression* temp = simple_expression(); // left expression

        if (nextToken == TokenType::OP_EQUALS || nextToken == TokenType::OP_NOT_EQUALS || nextToken == TokenType::OP_LESS ||
            nextToken == TokenType::OP_LESS_EQUAL || nextToken == TokenType::OP_GREATER || nextToken == TokenType::OP_GREATER_EQUAL) {

            Token operatorToken = match();

            temp = new Expr::Binary(temp, operatorToken, simple_expression());
        }

        return temp;
    }

    Expression* simple_expression() {
        Expression* temp;

        // optional (minus) sign
        if (nextToken == TokenType::OP_SUB) { // sign/unary minus
            Token opToken = match(TokenType::OP_SUB);
            temp = new Expr::Unary(opToken, term());
        }

        // main term
        temp = term();

        // add operations
        while (nextToken == TokenType::OP_ADD || nextToken == TokenType::OP_SUB || nextToken == TokenType::OP_OR) { // TODO: change to all strich operators
            Token opToken = match();
            temp = new Expr::Binary(temp, opToken, term());
        }

        return temp;
    }

    Expression* term() {
        // main factor
        Expression* temp = factor();

        // mult operations
        while (nextToken == TokenType::OP_MUL || nextToken == TokenType::OP_DIV || nextToken == TokenType::OP_INTEGER_DIV || nextToken == TokenType::OP_AND) {
            Token operatorToken = match();

            temp = new Expr::Binary(temp, operatorToken, factor());
        }

        return temp;
    }

    Expression* factor() {
        Expression* temp;

        switch (nextToken) {
            // arbitrary amount of "not"'s
            case TokenType::OP_NOT:
            {
                Token opToken = match();

                temp = new Expr::Unary(opToken, factor());
            } break;

            // groupings (with brackets)
            case TokenType::BRACKETS_OPEN:
            {
                std::cout << "[factor] found BRACKETS_OPEN" << std::endl;

                match(TokenType::BRACKETS_OPEN);
                temp = new Expr::Grouping(expression());
                match(TokenType::BRACKETS_CLOSING);

            } break;

            // literals
            case TokenType::LITERAL_INTEGER:
            case TokenType::LITERAL_REAL:
            case TokenType::LITERAL_STRING: 
            {
                std::cout << "[factor] found LITERAL (" << yytext << ")" << std::endl;

                Token literalToken = match();
                temp = new Expr::Literal{literalToken};
            } break;

            // identifiers and function calls
            case TokenType::IDENTIFIER:
            {
                Token identifierToken = match();

                // check if we have a function call (starts with opening bracket)
                if (nextToken == TokenType::BRACKETS_OPEN) {
                    // a function call
                    match(TokenType::BRACKETS_OPEN);

                    std::cout << "[factor] found FUNCTION CALL (" << yytext << ")" << std::endl;

                    // matching argument list (list of expressions)
                    std::vector<Expression*> argumentList;
                    if (nextToken != TokenType::BRACKETS_CLOSING) {
                        argumentList.push_back(expression());

                        while (nextToken == TokenType::COMMA) {
                            match(TokenType::COMMA); // consume the comma
                            argumentList.push_back(expression());
                        }
                    }

                    temp = new Expr::Call(identifierToken, argumentList); 

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