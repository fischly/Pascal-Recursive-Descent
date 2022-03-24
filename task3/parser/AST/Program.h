
#pragma once

#include <vector>

#include "Statement.h"
#include "Variable.h"
#include "Method.h"

class Program {
public:
    Program(Token identifier, std::vector<Variable*> declarations, std::vector<Method*> methods)
        : identifier{identifier}, declarations{declarations}, methods{methods}
    {}

    Token identifier;
    std::vector<Variable*> declarations;
    std::vector<Method*> methods;
};