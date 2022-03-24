
#include "Parser.h"

#include <iostream>
#include <string>
#include <sstream>
#include <list>

std::string parenthesize(std::string name, const std::list<Expression*>& expressions);


std::string renderExpression(Expression* exp) {

    if (ExpressionBinary* binaryExpression = dynamic_cast<ExpressionBinary*>(exp)) {
        // std::cout << "GOT BINARY EXPRESSION: operator = " << binaryExpression->op.lexeme << std::endl;
        return parenthesize(binaryExpression->op.lexeme, {binaryExpression->left, binaryExpression->right});
    } 
    else if (ExpressionCall* callExpression = dynamic_cast<ExpressionCall*>(exp)) {
        // std::cout << "GOT CALL EXPRESSION: callee = " << callExpression->callee.lexeme << std::endl;
        
        // convert the vector to list
        std::list<Expression*> exprList;
        std::copy(callExpression->arguments.begin(), callExpression->arguments.end(), std::back_inserter(exprList));

        return parenthesize(callExpression->callee.lexeme, exprList);
    }
    else if (ExpressionGrouping* groupingExpression = dynamic_cast<ExpressionGrouping*>(exp)) {
        // std::cout << "GOT GROUPING EXPRESSION! " << std::endl;
        return parenthesize("group", {groupingExpression->expression});
    }
    else if (ExpressionLiteral* literalExpression = dynamic_cast<ExpressionLiteral*>(exp)) {
        // std::cout << "GOT LITERAL EXPRESSION: " << literalExpression->token.lexeme << std::endl;
        return std::string(literalExpression->token.lexeme);
    }
    else if (ExpressionUnary* unaryExpression = dynamic_cast<ExpressionUnary*>(exp)) {
        return parenthesize(unaryExpression->op.lexeme, {unaryExpression->right});
    }

    return "";
}

std::string parenthesize(std::string name, const std::list<Expression*>& expressions) {
    std::stringstream ss;
    ss << "(" << name;

    for (auto const& exp : expressions) {
        ss << " ";
        ss << renderExpression(exp);
    }

    ss << ")";

    return ss.str();
}


int main(int argc, char **argv) {
    // yyin = fopen("test-code/literalExpression.pas", "r");

    std::cout << "Hello world from parser" << std::endl;

    Parser p;
    Expression* exp = p.expression();

    std::cout << "\n\n=====================================================\n\n";

    std::cout << renderExpression(exp);

    // p.match(TokenType::LITERAL_INTEGER);
   
    // int i;
    // while ((i = yylex())) {
    //     std::cout << "got token: " << TOKEN_NAMES[i] << " (lexem = '" << yytext << "')" << std::endl;
    // }
    // return 0;

    // fclose(yyin);
}