
#pragma once

#include <vector>

#include "Token.h"
#include "Expression.h"
#include "../../common/token-enum.h"

class ExpressionGrouping : public Expression {
public:
    ExpressionGrouping(Expression* expression) : expression{expression} {}

    Expression* expression;
};