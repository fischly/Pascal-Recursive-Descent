
#include "Parser.h"

#include <iostream>
#include <string>
#include <sstream>
#include <list>

std::string parenthesize(std::string name, const std::list<Expr::Expression*>& expressions);
std::string renderMethod(Method* method);

std::string renderExpression(Expr::Expression* exp) {

    if (Expr::Binary* binaryExpression = dynamic_cast<Expr::Binary*>(exp)) {
        // std::cout << "GOT BINARY EXPRESSION: operator = " << binaryExpression->op.lexeme << std::endl;
        return parenthesize(binaryExpression->op.lexeme, {binaryExpression->left, binaryExpression->right});
    } 
    else if (Expr::Call* callExpression = dynamic_cast<Expr::Call*>(exp)) {
        // std::cout << "GOT CALL EXPRESSION: callee = " << callExpression->callee.lexeme << std::endl;
        
        // convert the vector to list
        std::list<Expr::Expression*> exprList;
        std::copy(callExpression->arguments.begin(), callExpression->arguments.end(), std::back_inserter(exprList));

        return parenthesize(callExpression->callee.lexeme, exprList);
    }
    else if (Expr::Grouping* groupingExpression = dynamic_cast<Expr::Grouping*>(exp)) {
        // std::cout << "GOT GROUPING EXPRESSION! " << std::endl;
        return parenthesize("group", {groupingExpression->expression});
    }
    else if (Expr::Identifier* identifierExpression = dynamic_cast<Expr::Identifier*>(exp)) {
        // std::cout << "GOT IDENTIFIER EXPRESSION: " << identifierExpression->token.lexeme << std::endl;
        return std::string(identifierExpression->token.lexeme);
    }
    else if (Expr::Literal* literalExpression = dynamic_cast<Expr::Literal*>(exp)) {
        // std::cout << "GOT LITERAL EXPRESSION: " << literalExpression->token.lexeme << std::endl;
        return std::string(literalExpression->token.lexeme);
    }
    else if (Expr::Unary* unaryExpression = dynamic_cast<Expr::Unary*>(exp)) {
        return parenthesize(unaryExpression->op.lexeme, {unaryExpression->right});
    }

    return "";
}

std::string parenthesize(std::string name, const std::list<Expr::Expression*>& expressions) {
    std::stringstream ss;
    ss << "(" << name;

    for (auto const& exp : expressions) {
        ss << " ";
        ss << renderExpression(exp);
    }

    ss << ")";

    return ss.str();
}



std::string renderStatement(Stmt::Statement* stmt) {

    std::stringstream ss;

    if (Stmt::Assignment* assignStatement = dynamic_cast<Stmt::Assignment*>(stmt)) {
        ss << "(assign " << assignStatement->identifier.lexeme;
        if (assignStatement->arrayIndex != NULL) {
            ss << "[" << renderExpression(assignStatement->arrayIndex) << "]";
        }
        ss << " := " << renderExpression(assignStatement->value) << ")";
    } 
    else if (Stmt::Call* callStatement = dynamic_cast<Stmt::Call*>(stmt)) {
        ss << "(call " << callStatement->callee.lexeme;

        for (auto const& exp : callStatement->arguments) {
            ss << " ";
            ss << renderExpression(exp);
        }

        ss << ")";
    }
    else if (Stmt::If* ifStatement = dynamic_cast<Stmt::If*>(stmt)) {
        ss << "(if " << renderExpression(ifStatement->condition) << " (then " << renderStatement(ifStatement->thenBody) << ")";
        
        if (ifStatement->elseBody != NULL) {
            ss << " (else " << renderStatement(ifStatement->elseBody) << ")";
        }
        ss << ")";
    }
    else if (Stmt::While* whileStatement = dynamic_cast<Stmt::While*>(stmt)) {
        ss << "(while " << renderExpression(whileStatement->condition) << " (do " << renderStatement(whileStatement->body) << "))";
    }
    else if (Stmt::Block* blockStatement = dynamic_cast<Stmt::Block*>(stmt)) {
        ss << "(";

        for (auto const& stmt : blockStatement->statements) {
            ss << renderStatement(stmt);
        }
        
        ss << ")";
    }

    return ss.str();
}

std::string renderProgram(Program* prog) {
    std::stringstream ss;

    ss << "(program " << prog->identifier.lexeme << " (defs";

    for (const auto& varDecl : prog->declarations) {
        ss << " (" << varDecl->name.lexeme << ": ";

        if (Variable::VariableTypeSimple* simpleVar = dynamic_cast<Variable::VariableTypeSimple*>(varDecl->type)) {
            ss << simpleVar->typeName.lexeme;
        } else if (Variable::VariableTypeArray* arrayVar = dynamic_cast<Variable::VariableTypeArray*>(varDecl->type)) {
            ss << arrayVar->typeName.lexeme << "[" << arrayVar->startRange.lexeme << ".." << arrayVar->stopRange.lexeme << "]";
        }

        ss << ")";
    }

    ss << ")\n";

    // methods
    for (const auto& meth : prog->methods) {
        ss << renderMethod(meth) << "\n\n";
    }

    return ss.str();
}

std::string renderMethod(Method* method) {
    std::stringstream ss;

    ss << "(method " << method->identifier.lexeme << " (args";
    
    for (const auto& varArg : method->arguments) {
        ss << " (" << varArg->name.lexeme << ": ";

        if (Variable::VariableTypeSimple* simpleVar = dynamic_cast<Variable::VariableTypeSimple*>(varArg->type)) {
            ss << simpleVar->typeName.lexeme;
        } else if (Variable::VariableTypeArray* arrayVar = dynamic_cast<Variable::VariableTypeArray*>(varArg->type)) {
            ss << arrayVar->typeName.lexeme << "[" << arrayVar->startRange.lexeme << ".." << arrayVar->stopRange.lexeme << "]";
        }

        ss << ")";
    }
    
    
    ss << " (defs";

    for (const auto& varDecl : method->declarations) {
        ss << " (" << varDecl->name.lexeme << ": ";

        if (Variable::VariableTypeSimple* simpleVar = dynamic_cast<Variable::VariableTypeSimple*>(varDecl->type)) {
            ss << simpleVar->typeName.lexeme;
        } else if (Variable::VariableTypeArray* arrayVar = dynamic_cast<Variable::VariableTypeArray*>(varDecl->type)) {
            ss << arrayVar->typeName.lexeme << "[" << arrayVar->startRange.lexeme << ".." << arrayVar->stopRange.lexeme << "]";
        }

        ss << ")";
    }

    ss << ")\n";

    ss << renderStatement(method->block);

    return ss.str();
}


int main(int argc, char **argv) {
    // yyin = fopen("test-code/literalExpression.pas", "r");

    std::cout << "Hello world from parser" << std::endl;

    Parser p;
    // Expr::Expression* exp = p.expression();
    // Stmt::Statement* stmt = p.statement();
    Program* prog = p.program();

    std::cout << "\n\n=====================================================\n\n";

    std::cout << renderProgram(prog);

    // p.match(TokenType::LITERAL_INTEGER);
   
    // int i;
    // while ((i = yylex())) {
    //     std::cout << "got token: " << TOKEN_NAMES[i] << " (lexem = '" << yytext << "')" << std::endl;
    // }
    // return 0;

    // fclose(yyin);
}