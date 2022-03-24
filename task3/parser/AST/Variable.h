
#pragma once

#include <vector>

#include "Statement.h"
#include "Token.h"

using Stmt::Statement;

class Variable {
public:

    class VariableType {
    public:
        virtual ~VariableType() = default;
    };

    class VariableTypeSimple : public VariableType {
    public:
        VariableTypeSimple(Token typeName) : typeName{typeName} {}

        Token typeName;
    };

    class VariableTypeArray : public VariableType {
    public:
        VariableTypeArray(Token typeName, Token startRange, Token stopRange) 
            : typeName{typeName}, startRange{startRange}, stopRange{stopRange}
        {}        

        Token typeName;
        Token startRange;
        Token stopRange;
    };


    Variable(Token name, VariableType* type) : name{name}, type{type} {}

    Token name;
    VariableType* type;
};