
#include <iostream>
#include <exception>
#include <utility>
#include <algorithm>

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
        
//         std::cout << "updated next token from " << TOKEN_NAMES[nextToken] << " (\"" << yytext << "\") to ";
        nextToken = static_cast<TokenType>(yylex());
//         std::cout << TOKEN_NAMES[nextToken] << " (\"" << yytext << "\")" << std::endl;

        return consumedToken;
    }


    void checkInput(NonTerminal rule) {
        // check if next token is in rule set
        std::vector<TokenType> firstSet = FIRSTSETS[rule];

        if (std::find(firstSet.begin(), firstSet.end(), nextToken) == firstSet.end()) {
            // display error
            std::cout << "Error on rule " << RULE_NAMES[rule] << ":";
            throw SyntaxException(nextToken, firstSet, yylineno);
        }

    }

    /* =========================================================================================================================== */
    /* ========= Program ========================================================================================================= */
    /* =========================================================================================================================== */

    Program* start() {
        checkInput(NonTerminal::START);

        match(TokenType::PROGRAM);
        Token programIdentifier = match(TokenType::IDENTIFIER);
        match(TokenType::SEMICOLON);

        // declarations
        std::vector<Variable*> decls;
        if (nextToken == TokenType::VAR) {
             decls = varDecl();
        }

        // methods
        std::vector<Method*> meths = subProgList();

        // match main
        Stmt::Block* main = compStmt();

        match(TokenType::DOT);

        return new Program(programIdentifier, decls, meths, main);
    }

    std::vector<Method*> subProgList() {
        checkInput(NonTerminal::SUBPROGLIST);

        std::vector<Method*> meths;
        while (nextToken == TokenType::FUNCTION || nextToken == TokenType::PROCEDURE) {
            meths.push_back(method());
        }
        return meths;
    }

    /* --------------- Declarations --------------------- */
    std::vector<Variable*> varDecl() {
        checkInput(NonTerminal::VARDEC);

        match(TokenType::VAR);

        std::vector<Variable*> declarations;
        declarations = varDecList();

        return declarations;
    }

    std::vector<Variable*> varDecList() {
        checkInput(NonTerminal::VARDECLIST);

        std::vector<Variable*> declarations = identListType();
        match(TokenType::SEMICOLON);

        // check for more lines
        while (nextToken == TokenType::IDENTIFIER) {
            std::vector<Variable*> newDeclarations = identListType();
            match(TokenType::SEMICOLON);
            // copy the new ones to our declaration list
            declarations.insert(declarations.end(), newDeclarations.begin(), newDeclarations.end());
        }

        return declarations;
    }


    std::vector<Variable*> identListType() {
        checkInput(NonTerminal::IDENTLISTTYPE);

        std::vector<Token> variableNames = identList();

        match(TokenType::COLON);

        Variable::VariableType* variableType = type();

        std::vector<Variable*> declarations;
        for (Token identifier : variableNames) {
            declarations.push_back(new Variable(identifier, variableType));
        }

        return declarations;
    }

    std::vector<Token> identList() {
        checkInput(NonTerminal::IDENTLIST);

        std::vector<Token> variableNames;
        variableNames.push_back(match(TokenType::IDENTIFIER));
        while (nextToken == TokenType::COMMA) {
            match(TokenType::COMMA);
            variableNames.push_back(match(TokenType::IDENTIFIER));
        }
        return variableNames;
    }

    Token simpleType() {
        checkInput(NonTerminal::SIMPLETYPE);

        return match();
    }

    Variable::VariableType* type() {
        checkInput(NonTerminal::TYPE);

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

            Token typeName = simpleType();

            temp = new Variable::VariableTypeArray(typeName, startRange, stopRange);
        } else {
            // standard type
            temp = new Variable::VariableTypeSimple(simpleType());
        }

        return temp;
    }




    /* =========================================================================================================================== */
    /* ========= Methods ========================================================================================================= */
    /* =========================================================================================================================== */

    std::vector<Variable*> parList() {
        checkInput(NonTerminal::PARLIST);

        std::vector<Variable*> args;
        if (nextToken == TokenType::IDENTIFIER) {
            args = identListType();

            while (nextToken == TokenType::SEMICOLON) {
                match(TokenType::SEMICOLON);
                std::vector<Variable*> newArgs = identListType();
                args.insert(args.end(), newArgs.begin(), newArgs.end());
            }
        }

        return args;
    }



    MethodHead* subProgHead() {
        checkInput(NonTerminal::SUBPROGHEAD);

        if (nextToken != TokenType::FUNCTION && nextToken != TokenType::PROCEDURE) {
            std::stringstream ss;
            ss << "Expected method declaration (starting with either 'function' or 'procedure') but got " << TOKEN_NAMES[nextToken] << " at line " << yylineno;
            throw SyntaxException(ss.str().c_str());
        }

        Token methodKeyword = match(); // consume FUNCTION or PROCEDURE token
        Token methodIdentifier = match(TokenType::IDENTIFIER);

        std::vector<Variable*> args;
        if (nextToken == TokenType::BRACKETS_OPEN) {
            match(TokenType::BRACKETS_OPEN);
             args = parList();
            match(TokenType::BRACKETS_CLOSING);
        }

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

            returnType = type();
        }
        match(TokenType::SEMICOLON);

        // throw exception when a function has no return type
        if (returnType == NULL && methodKeyword.type == TokenType::FUNCTION) {
            std::stringstream ss;
            ss << "Function must have a return type at line " << yylineno;
            throw SyntaxException(ss.str().c_str());
        }

        return new MethodHead(methodIdentifier, args, returnType);
    }

    Method* method() {
        // no input check, since this method does not reflect a rule

        MethodHead* methHead = subProgHead();

        // declarations
        std::vector<Variable*> decls;
        if (nextToken == TokenType::VAR) {
             decls = varDecl();
        }

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

        return new Method(methHead, decls, methodBlock);
    }


    /* =========================================================================================================================== */
    /* ========= Statements  ===================================================================================================== */
    /* =========================================================================================================================== */

    Statement* statement() {
        checkInput(NonTerminal::STATEMENT);

        Statement* temp;

        switch (nextToken) {
            case TokenType::BEGIN_: temp = compStmt(); break;
            case TokenType::WHILE: temp = whileStmt(); break;
            case TokenType::IF: temp = ifStmt(); break;
            case TokenType::IDENTIFIER: {
                Token identifierToken = match(TokenType::IDENTIFIER);
                temp = statement2(identifierToken);
            } break;
            default: {
                std::stringstream ss;
                ss << "Expected statement, but got token '" << TOKEN_NAMES[nextToken] << "' at line " << yylineno;
                throw SyntaxException(ss.str().c_str());
            }; break;
        }

        return temp;
    }

    std::vector<Statement*> stmtList() {
        checkInput(NonTerminal::STMTLIST);

        std::vector<Statement*> statementsInBlock;

        if (nextToken != TokenType::END_) {
            statementsInBlock.push_back(statement());

            while (nextToken != TokenType::END_) {
                match(TokenType::SEMICOLON);

                statementsInBlock.push_back(statement());
            }
        }

        return statementsInBlock;
    }

    Stmt::Block* compStmt() {
        checkInput(NonTerminal::COMPSTMT);

        match(TokenType::BEGIN_);
        std::vector<Statement*> statementsInBlock = stmtList();
        match(TokenType::END_);

        return new Stmt::Block(statementsInBlock);
    }

    Stmt::While* whileStmt() {
        checkInput(NonTerminal::WHILESTMT);

        match(TokenType::WHILE);

        Expression* condition = expression();
        match(TokenType::DO);
        Statement* body = statement();

        return new Stmt::While(condition, body);
    }

    Stmt::If* ifStmt() {
        checkInput(NonTerminal::IFSTMT);

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

    std::vector<Expression*> exprList() {
        checkInput(NonTerminal::EXPRLIST);

        std::vector<Expression*> expressionList;
        expressionList.push_back(expression());

        while (nextToken == TokenType::COMMA) {
            match(TokenType::COMMA);
            expressionList.push_back(expression());
        }

        return expressionList;
    }

    std::vector<Expression*> params() {
        checkInput(NonTerminal::PARAMS);

        match(TokenType::BRACKETS_OPEN);
        std::vector<Expression*> expressionList = exprList();
        match(TokenType::BRACKETS_CLOSING);

        return expressionList;
    }

    Stmt::Call* procCall(Token identifierToken) {
        checkInput(NonTerminal::PROCCALL);

        if (nextToken == TokenType::BRACKETS_OPEN) {
            return new Stmt::Call(identifierToken, params());
        }

        return NULL;
    }

    std::pair<Expression*, Expression*> index() {
        checkInput(NonTerminal::INDEX);

        match(TokenType::SQUARE_OPEN);

        Expression* arrayIndexValue1 = expression();
        Expression* arrayIndexValue2 = NULL;

        if (nextToken == TokenType::RANGE_DOTS) {
            match(TokenType::RANGE_DOTS);

            arrayIndexValue2 = expression();
        }

        match(TokenType::SQUARE_CLOSING);

        return std::make_pair(arrayIndexValue1, arrayIndexValue2);
    }

    // index -> [ expr

    Stmt::Assignment* assignStmt (Token identifierToken) {
        checkInput(NonTerminal::ASSIGNSTMT);

        std::pair<Expression*, Expression*> arrayIndex;
        if (nextToken == TokenType::SQUARE_OPEN) {
           arrayIndex = index();
        }

        match(TokenType::OP_ASSIGNMENT);

        Expression* assignmentValue = expression();

        return new Stmt::Assignment(identifierToken, arrayIndex.first, arrayIndex.second, assignmentValue);
    }
    

    Statement* statement2(Token identifierToken) {
        checkInput(NonTerminal::STATEMENT2);

        Statement* temp;

        // method call
        if (nextToken == TokenType::BRACKETS_OPEN) {
            temp = procCall(identifierToken);
        }
        // assignment
        else {
            temp = assignStmt (identifierToken);
        }

        return temp;
    }





    /* =========================================================================================================================== */
    /* ========= Expressions ===================================================================================================== */
    /* =========================================================================================================================== */

    Token relOp() {
        checkInput(NonTerminal::RELOP);

        return match();
    }

    Token addOp() {
        checkInput(NonTerminal::ADDOP);

        return match();
    }

    Token mulOp() {
        checkInput(NonTerminal::MULOP);

        return match();
    }

    Expression* expression() {
        checkInput(NonTerminal::EXPR);

        Expression* temp = simpleExpression(); // left expression

        if (nextToken == TokenType::OP_EQUALS || nextToken == TokenType::OP_NOT_EQUALS || nextToken == TokenType::OP_LESS ||
            nextToken == TokenType::OP_LESS_EQUAL || nextToken == TokenType::OP_GREATER || nextToken == TokenType::OP_GREATER_EQUAL) {

            Token operatorToken = relOp();
            Expression* rightSide = simpleExpression();

            temp = new Expr::Binary(temp, operatorToken, rightSide);
        }

        return temp;
    }

    Expression* simpleExpression() {
        checkInput(NonTerminal::SIMPLEEXPR);

        Expression* temp = term();

        // add operations
        while (nextToken == TokenType::OP_ADD || nextToken == TokenType::OP_SUB || nextToken == TokenType::OP_OR) {
            Token opToken = addOp();
            Expression* rightSide = term();

            temp = new Expr::Binary(temp, opToken, rightSide);
        }

        return temp;
    }

    Expression* term() {
        checkInput(NonTerminal::TERM);

        // main factor
        Expression* temp = factor();

        // mult operations
        while (nextToken == TokenType::OP_MUL || nextToken == TokenType::OP_DIV || nextToken == TokenType::OP_INTEGER_DIV || nextToken == TokenType::OP_AND) {
            Token operatorToken = mulOp();
            Expression* rightSide = factor();

            temp = new Expr::Binary(temp, operatorToken, rightSide);
        }

        return temp;
    }

    Expression* factor() {
        checkInput(NonTerminal::FACTOR);

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
                    std::vector<Expression*> argumentList = params();
                    temp = new Expr::Call(identifierToken, argumentList);
                } else if (nextToken == TokenType::SQUARE_OPEN) {
                    std::pair<Expression*, Expression*> indexValues = index();
                    if (indexValues.first == NULL) {
                    }
                    temp = new Expr::Identifier(identifierToken, indexValues.first); // TODO: store end index
                } else {
                    temp = new Expr::Identifier(identifierToken, NULL);
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