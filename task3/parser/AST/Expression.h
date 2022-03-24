
#pragma once

#include <vector>


#include "Token.h"
#include "../../common/token-enum.h"

namespace Expr {
    /* Base class for all expressions */
    class Expression {
    public:
        virtual ~Expression() = default; // adding virtual destructor to make Expression class polymorphic
    };
    

    class Binary : public Expression {
    public:
        Binary(Expression* left, Token op, Expression* right) 
            : left{left}, op{op}, right{right}
        {}

        Expression* left;
        Token op;
        Expression* right;
    };

    class Call : public Expression {
    public:
        Call(Token callee, std::vector<Expression*> arguments)
            : callee{callee}, arguments{arguments}
        {}

        Token callee;
        std::vector<Expression*> arguments;
    };

    class Grouping : public Expression {
    public:
        Grouping(Expression* expression) : expression{expression} {}

        Expression* expression;
    };

    class Literal : public Expression {
    public:
        Literal(Token token) : token{token} {}

        Token token;
    };

    class Unary : public Expression {
    public:
        Unary(Token op, Expression* right) : op{op}, right{right} {}

        Token op;
        Expression* right;
    };
}

