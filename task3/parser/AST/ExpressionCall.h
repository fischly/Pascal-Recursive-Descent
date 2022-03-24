
#pragma once

#include <vector>

#include "Token.h"
#include "Expression.h"
#include "../../common/token-enum.h"

class ExpressionCall : public Expression {
public:
    Token callee;
    std::vector<Expression*> arguments;
};