
#pragma once

#include <vector>


#include "Token.h"
#include "../../common/token-enum.h"

namespace Expr {
    /* forward declarations */
    class Binary; class Call; class Grouping;
    class Identifier; class Literal; class Unary;

    /* expression visitor */
    class Visitor {
    public:
        virtual ~Visitor() = default;

        virtual void visitBinary(Binary* expr) {};
        virtual void visitCall(Call* expr) {};
        virtual void visitGrouping(Grouping* expr) {};
        virtual void visitIdentifier(Identifier* expr) {};
        virtual void visitLiteral(Literal* expr) {};
        virtual void visitUnary(Unary* expr) {};
    };

    /* Base class for all expressions */
    class Expression {
    public:
        virtual ~Expression() = default;
        virtual void accept(Visitor* visitor) = 0;
    };
    

    /* different types of expressions */
    class Binary : public Expression {
    public:
        Binary(Expression* left, Token op, Expression* right) 
            : left{left}, op{op}, right{right}
        {}

        Expression* left;
        Token op;
        Expression* right;

        void accept(Visitor* visitor) { visitor->visitBinary(this); }
    };

    class Call : public Expression {
    public:
        Call(Token callee, std::vector<Expression*> arguments)
            : callee{callee}, arguments{arguments}
        {}

        Token callee;
        std::vector<Expression*> arguments;

        void accept(Visitor* visitor) { visitor->visitCall(this); }
    };

    class Grouping : public Expression {
    public:
        Grouping(Expression* expression) : expression{expression} {}
        
        Expression* expression;

        void accept(Visitor* visitor) { visitor->visitGrouping(this); }
    };

    class Identifier : public Expression {
    public:
        Identifier(Token token, Expression* arrayIndexExpression) 
            : token{token}, arrayIndexExpression{arrayIndexExpression} {}

        Token token;
        Expression* arrayIndexExpression;

        void accept(Visitor* visitor) { visitor->visitIdentifier(this); }
    };

    class Literal : public Expression {
    public:
        Literal(Token token) : token{token} {}
        
        Token token;

        void accept(Visitor* visitor) { visitor->visitLiteral(this); }
    };

    class Unary : public Expression {
    public:
        Unary(Token op, Expression* right) : op{op}, right{right} {}

        Token op;
        Expression* right;

        void accept(Visitor* visitor) { visitor->visitUnary(this); }
    };

}

