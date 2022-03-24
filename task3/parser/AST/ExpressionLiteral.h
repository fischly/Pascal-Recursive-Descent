
#pragma once

#include <vector>

#include "Token.h"
#include "Expression.h"
#include "../../common/token-enum.h"

class ExpressionLiteral : public Expression {
public:
    ExpressionLiteral(Token token) : token{token} {}

    Token token;
};