
#include "Parser.h"

#include <iostream>
#include <string>
#include <sstream>
#include <list>


int main(int argc, char **argv) {
    Parser p;

    Program* prog;
    try {
         prog = p.program();
    } catch (SyntaxException ex) {
        std::cout << "Syntax error: " << ex.what() << std::endl;
        return -1;
    }

    AST2Text ast2text;
    prog->accept(&ast2text);
    
    std::cout << ast2text.getResult() << std::endl;

    delete prog;
}