
#pragma once

#include <vector>

#include "Statement.h"
#include "Variable.h"
#include "MethodHead.h"


class Method {
public:
    /* method visitor */
    class Visitor {
    public:
        virtual ~Visitor() = default;

        virtual void visitMethod(Method* method) {};
    };


    Method(MethodHead* methodHead,
           std::vector<Variable*> declarations,
           Stmt::Block* block)
        : methodHead{methodHead}, declarations{declarations}, block{block}
    {}
    ~Method() {
        for (auto& decl : declarations) {
            delete decl;
        }

        delete block;
    }

    MethodHead* methodHead;
    std::vector<Variable*> declarations;
    Stmt::Block* block;

    void accept(Visitor* visitor) { visitor->visitMethod(this); }
};