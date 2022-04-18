
#include <iostream>
#include <exception>

#include "../common/token-enum.h"
#include "../lexer/lex.yy.c"

#include "SyntaxException.h"

#include "AST/Expression.h"
#include "AST/Statement.h"
#include "AST/Variable.h"
#include "AST/Method.h"
#include "AST/Program.h"

#include "AST/Visitors/AST2Text.h"
#include "AST/Visitors/AST2Dot.h"

#include "error-recovery/FirstSets.h"

// extern int yylex();
// extern int yylineno;
// extern char* yytext;
// extern FILE *yyin;

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
    /* ========= Program ========================================================================================================= */
    /* =========================================================================================================================== */

    Program* program() {
        match(TokenType::PROGRAM);
        Token programIdentifier = match(TokenType::IDENTIFIER);
        match(TokenType::SEMICOLON);

        // declarations
        std::vector<Variable*> decls = declarations();

        // methods
        std::vector<Method*> meths;
        while (nextToken == TokenType::FUNCTION || nextToken == TokenType::PROCEDURE) {
            meths.push_back(method());
        }

        // match main
        Stmt::Block* main = statement_block();

        match(TokenType::DOT);

        return new Program(programIdentifier, decls, meths, main);
    }

    /* --------------- Declarations --------------------- */
    std::vector<Variable*> declarations() {
        std::vector<Variable*> declarations;

        if (nextToken == TokenType::VAR) {
            match(TokenType::VAR);
            declarations = declaration_line();
            match(TokenType::SEMICOLON);


            // check for more lines
            while (nextToken == TokenType::IDENTIFIER) {
                std::vector<Variable*> newDeclarations = declaration_line();
                match(TokenType::SEMICOLON);
                // copy the new ones to our declaration list
                declarations.insert(declarations.end(), newDeclarations.begin(), newDeclarations.end());
            }
        }

        return declarations;
    }

    std::vector<Variable*> declaration_line() {
        std::vector<Variable*> declarations;

        std::vector<Token> variableNames;
        variableNames.push_back(match(TokenType::IDENTIFIER));
        while (nextToken == TokenType::COMMA) {
            match(TokenType::COMMA);
            variableNames.push_back(match(TokenType::IDENTIFIER));
        }

        match(TokenType::COLON);

        // type
        Variable::VariableType* variableType = variable_type();


        for (Token identifier : variableNames) {
            declarations.push_back(new Variable(identifier, variableType));
        }

        return declarations;
    }

    Variable::VariableType* variable_type() {
        Variable::VariableType* temp;

        if (nextToken == TokenType::ARRAY) {
            // array type
            match(TokenType::ARRAY);
            match(TokenType::SQUARE_OPEN);

            Token startRange = match(TokenType::LITERAL_INTEGER);
            match(TokenType::RANGE_DOTS);
            Token stopRange = match(TokenType::LITERAL_INTEGER);

            match(TokenType::SQUARE_CLOSING);

            match(TokenType::OF);

            Token typeName = simple_type();

            temp = new Variable::VariableTypeArray(typeName, startRange, stopRange);
        } else {
            // standard type
            temp = new Variable::VariableTypeSimple(simple_type());
        }

        return temp;
    }

    Token simple_type() {
        if (nextToken == TokenType::INTEGER || nextToken == TokenType::REAL || nextToken == TokenType::BOOLEAN) {
            return match();
        } else {
            std::stringstream ss;
            ss << "Expected standard type (integer, real or boolean), but got " << TOKEN_NAMES[nextToken] << " at line " << yylineno;
            throw SyntaxException(ss.str().c_str());
        }
    }


    /* =========================================================================================================================== */
    /* ========= Methods ========================================================================================================= */
    /* =========================================================================================================================== */

    Method* method() {
        if (nextToken != TokenType::FUNCTION && nextToken != TokenType::PROCEDURE) {
            std::stringstream ss;
            ss << "Expected method declaration (starting with either 'function' or 'procedure') but got " << TOKEN_NAMES[nextToken] << " at line " << yylineno;
            throw SyntaxException(ss.str().c_str());
        }

        Token methodKeyword = match(); // consume FUNCTION or PROCEDURE token
        Token methodIdentifier = match(TokenType::IDENTIFIER);
        
        // arguments
        match(TokenType::BRACKETS_OPEN);

        std::vector<Variable*> args;
        if (nextToken == TokenType::IDENTIFIER) {
            args = declaration_line();

            while (nextToken == TokenType::SEMICOLON) {
                match(TokenType::SEMICOLON);
                std::vector<Variable*> newArgs = declaration_line();
                args.insert(args.end(), newArgs.begin(), newArgs.end());
            }
        }
        
        match(TokenType::BRACKETS_CLOSING);

        // return type
        Variable::VariableType* returnType = NULL;
        if (nextToken == TokenType::COLON) {
            // throw exception when a procedure has a return type
            if (methodKeyword.type == TokenType::PROCEDURE) {
                std::stringstream ss;
                ss << "Procedure cannot have a return type at line " << yylineno;
                throw SyntaxException(ss.str().c_str());
            }

            // method with return value
            match(TokenType::COLON);

            returnType = variable_type();
        }
        match(TokenType::SEMICOLON);

        // throw exception when a function has no return type
        if (returnType == NULL && methodKeyword.type == TokenType::FUNCTION) {
            std::stringstream ss;
            ss << "Function must have a return type at line " << yylineno;
            throw SyntaxException(ss.str().c_str());
        }

        // declarations
        std::vector<Variable*> decls = declarations();

        // block
        match(TokenType::BEGIN_);

        std::vector<Statement*> statementsInBlock;

        if (nextToken != TokenType::END_) {
            statementsInBlock.push_back(statement());

            while (nextToken != TokenType::END_) {
                match(TokenType::SEMICOLON);

                statementsInBlock.push_back(statement());
            }
        }
        Stmt::Block* methodBlock = new Stmt::Block(statementsInBlock);

        match(TokenType::END_);
        match(TokenType::SEMICOLON);

        return new Method(methodIdentifier, args, decls, methodBlock, returnType);
    }


    /* =========================================================================================================================== */
    /* ========= Statements  ===================================================================================================== */
    /* =========================================================================================================================== */

    Stmt::Block* statement_block() {
        match(TokenType::BEGIN_);

        std::vector<Statement*> statementsInBlock;

        if (nextToken != TokenType::END_) {
            statementsInBlock.push_back(statement());

            while (nextToken != TokenType::END_) {
                match(TokenType::SEMICOLON);

                statementsInBlock.push_back(statement());
            }
        }

        match(TokenType::END_);

        return new Stmt::Block(statementsInBlock);
    }

    Stmt::While* statement_while() {
        match(TokenType::WHILE);

        Expression* condition = expression();
        match(TokenType::DO);
        Statement* body = statement();

        return new Stmt::While(condition, body);
    }

    Stmt::If* statment_if() {
        match(TokenType::IF);

        Expression* condition = expression();
        match(TokenType::THEN);
        Statement* thenBody = statement();
        Statement* elseBody = NULL;

        if (nextToken == TokenType::ELSE) {
            match(TokenType::ELSE);

            elseBody = statement();
        }

        return new Stmt::If(condition, thenBody, elseBody);
    }

    Stmt::Call* statement_method_call(Token identifierToken) {
         match(TokenType::BRACKETS_OPEN);

        std::vector<Expression*> argumentList;
        if (nextToken != TokenType::BRACKETS_CLOSING) {
            argumentList.push_back(expression());

            while (nextToken == TokenType::COMMA) {
                match(TokenType::COMMA);
                argumentList.push_back(expression());
            }
        }

        match(TokenType::BRACKETS_CLOSING);

        return new Stmt::Call(identifierToken, argumentList); 
    }

    Stmt::Assignment* statement_assignment(Token identifierToken) {
        Expression* arrayIndexValue = NULL;
        if (nextToken == TokenType::SQUARE_OPEN) {
            match(TokenType::SQUARE_OPEN);
            arrayIndexValue = expression();
            match(TokenType::SQUARE_CLOSING);
        }

        match(TokenType::OP_ASSIGNMENT);

        Expression* assignmentValue = expression();

        return new Stmt::Assignment(identifierToken, arrayIndexValue, assignmentValue);
    }
    
    

    Statement* statement() {
        Statement* temp;

        switch (nextToken) {
            case TokenType::BEGIN_: temp = statement_block(); break;
            case TokenType::WHILE: temp = statement_while(); break;
            case TokenType::IF: temp = statment_if(); break;
            case TokenType::IDENTIFIER: {
                Token identifierToken = match(TokenType::IDENTIFIER);
            
                // method call
                if (nextToken == TokenType::BRACKETS_OPEN) {
                    temp = statement_method_call(identifierToken);
                }
                // assignment
                else {
                    temp = statement_assignment(identifierToken);
                }
            } break;
            default: {
                std::stringstream ss;
                ss << "Expected statement, but got token '" << TOKEN_NAMES[nextToken] << "' at line " << yylineno;
                throw SyntaxException(ss.str().c_str());
            }; break;
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
            Expression* rightSide = simple_expression();

            temp = new Expr::Binary(temp, operatorToken, rightSide);
        }

        return temp;
    }

    Expression* simple_expression() {
        Expression* temp = term();

        // add operations
        while (nextToken == TokenType::OP_ADD || nextToken == TokenType::OP_SUB || nextToken == TokenType::OP_OR) {
            Token opToken = match();
            Expression* rightSide = term();

            temp = new Expr::Binary(temp, opToken, rightSide);
        }

        return temp;
    }

    Expression* term() {
        // main factor
        Expression* temp = factor();

        // mult operations
        while (nextToken == TokenType::OP_MUL || nextToken == TokenType::OP_DIV || nextToken == TokenType::OP_INTEGER_DIV || nextToken == TokenType::OP_AND) {
            Token operatorToken = match();
            Expression* rightSide = factor();

            temp = new Expr::Binary(temp, operatorToken, rightSide);
        }

        return temp;
    }

    Expression* factor() {
        Expression* temp;

        switch (nextToken) {
            // arbitrary amount of "not"'s or "-"'s
            case TokenType::OP_SUB:
            case TokenType::OP_NOT:
            {
                Token opToken = match();

                temp = new Expr::Unary(opToken, factor());
            } break;

            // groupings (with brackets)
            case TokenType::BRACKETS_OPEN:
            {
                match(TokenType::BRACKETS_OPEN);
                temp = new Expr::Grouping(expression());
                match(TokenType::BRACKETS_CLOSING);

            } break;

            // literals
            case TokenType::LITERAL_INTEGER:
            case TokenType::LITERAL_REAL:
            case TokenType::LITERAL_STRING: 
            case TokenType::LITERAL_TRUE:
            case TokenType::LITERAL_FALSE:
            {
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
                    Expression* arrayIndexExpression = NULL;

                    if (nextToken == TokenType::SQUARE_OPEN) {
                        match(TokenType::SQUARE_OPEN);
                        arrayIndexExpression = expression();
                        match(TokenType::SQUARE_CLOSING);

                    }

                    temp = new Expr::Identifier(identifierToken, arrayIndexExpression);
                }
            } break;
            default: 
            {
                std::stringstream ss;
                ss << "Unexpected token (" << TOKEN_NAMES[nextToken] << ") at line " << yylineno;
                throw SyntaxException(ss.str().c_str());
            } break;
        }

        return temp;
    }

};