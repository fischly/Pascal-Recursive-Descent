
#pragma once

#include <vector>

#include "Statement.h"
#include "Variable.h"


class Method {
public:
    Method(Token identifier, std::vector<Variable*> arguments, std::vector<Variable*> declarations, Stmt::Block* block, Token* returnType)
        : identifier{identifier}, arguments{arguments}, declarations{declarations}, block{block}, returnType{returnType}
    {}

    Token identifier;
    std::vector<Variable*> arguments;
    std::vector<Variable*> declarations;
    Stmt::Block* block;
    Token* returnType; // TODO: also allow array, currently only standard types are allowed
};