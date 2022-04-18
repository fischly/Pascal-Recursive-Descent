
#pragma once

#include <vector>

#include "Statement.h"
#include "Variable.h"


class MethodHead {
public:

    MethodHead(Token identifier,
           std::vector<Variable*> arguments,
           Variable::VariableType* returnType)
        : identifier{identifier}, arguments{arguments}, returnType{returnType}
    {}
    ~MethodHead() {
        for (auto& arg : arguments) {
            delete arg;
        }

        delete returnType;
    }

    Token identifier;
    std::vector<Variable*> arguments;
    Variable::VariableType* returnType;
};