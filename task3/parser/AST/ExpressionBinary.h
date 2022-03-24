
#pragma once

#include "Token.h"
#include "Expression.h"
#include "../../common/token-enum.h"

class ExpressionBinary : public Expression {
public:
    ExpressionBinary(Expression* left, Token op, Expression* right) 
        : left{left}, op{op}, right{right}
    {}

    Expression* left;
    Token op;
    Expression* right;
};