
#pragma once

#include <vector>

#include "Statement.h"
#include "Variable.h"


class Method {
public:
    /* method visitor */
    class Visitor {
    public:
        virtual ~Visitor() = default;

        virtual void visitMethod(Method* method) {};
    };


    Method(Token identifier, std::vector<Variable*> arguments, std::vector<Variable*> declarations, Stmt::Block* block, Token* returnType)
        : identifier{identifier}, arguments{arguments}, declarations{declarations}, block{block}, returnType{returnType}
    {}

    Token identifier;
    std::vector<Variable*> arguments;
    std::vector<Variable*> declarations;
    Stmt::Block* block;
    Token* returnType; // TODO: also allow array, currently only standard types are allowed

    void accept(Visitor* visitor) { visitor->visitMethod(this); }
};