
#include "Parser.h"

#include <iostream>
#include <string>
#include <sstream>
#include <list>


int main(int argc, char **argv) {
    // yyin = fopen("test-code/literalExpression.pas", "r");

    std::cout << "Hello world from parser" << std::endl;

    Parser p;
    // Expr::Expression* exp = p.expression();
    // Stmt::Statement* stmt = p.statement();
    Program* prog = p.program();

    std::cout << "\n\n=====================================================\n\n";

    // std::cout << renderProgram(prog);
    AST2Text ast2text;
    // exp->accept(&ast2text);
    prog->accept(&ast2text);

    std::cout << ast2text.getResult() << std::endl;

    // p.match(TokenType::LITERAL_INTEGER);
   
    // int i;
    // while ((i = yylex())) {
    //     std::cout << "got token: " << TOKEN_NAMES[i] << " (lexem = '" << yytext << "')" << std::endl;
    // }
    // return 0;

    // fclose(yyin);
}