
#pragma once

#include "Token.h"
#include "Expression.h"
#include "../../common/token-enum.h"

class ExpressionUnary : public Expression {
public:
    Token op;
    Expression* right;
};