
#pragma once

#include <vector>

#include "Expression.h"
#include "Token.h"

using Expr::Expression;

namespace Stmt {
    /* Base class */
    class Statement {
    public:
        virtual ~Statement() = default; // adding virtual destructor to make Expression class polymorphic
    };

    class Assignment {
    public:
        Assignment(Token identifier, Expression* arrayIndex, Expression* value) 
            : identifier{identifier}, arrayIndex{arrayIndex}, value{value}
        {}

        Token identifier;
        Expression* arrayIndex;
        Expression* value;
    };

    class Call {
    public:
        Call(Token callee, std::vector<Expression*> arguments)
            : callee{callee}, arguments{arguments}
        {}

        Token callee;
        std::vector<Expression*> arguments;
    };

    class If {
    public:
        If(Expression* condition, Statement* thenBody, Statement* elseBody)
            : condition{condition}, thenBody{thenBody}, elseBody{elseBody}
        {}

        Expression* condition;
        Statement* thenBody;
        Statement* elseBody;
    };

    class While {
    public:
        While(Expression* condition, Statement* body)
            : condition{condition}, body{body}
        {}

        Expression* condition;
        Statement* body;
    };

    class Block {
    public:
        Block(std::vector<Statement*> statements)
            : statements{statements}
        {}

        std::vector<Statement*> statements;
    };

};

