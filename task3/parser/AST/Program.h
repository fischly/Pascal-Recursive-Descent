
#pragma once

#include <vector>

#include "Statement.h"
#include "Variable.h"
#include "Method.h"

class Program {
public:
    /* program visitor */
    class Visitor {
    public:
        virtual ~Visitor() = default;

        virtual void visitProgram(Program* prog) {};
    };

    Program(Token identifier, std::vector<Variable*> declarations, std::vector<Method*> methods)
        : identifier{identifier}, declarations{declarations}, methods{methods}
    {}

    Token identifier;
    std::vector<Variable*> declarations;
    std::vector<Method*> methods;

    void accept(Visitor* visitor) { visitor->visitProgram(this); }
};