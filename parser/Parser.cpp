
#include "Parser.h"

#include <iostream>
#include <string>
#include <sstream>
#include <list>


int main(int argc, char **argv) {

    std::cout << "Hello world from parser" << std::endl;

    Parser p;
    Program* prog = p.program();

    AST2Dot ast2text;
    prog->accept(&ast2text);
    
    std::cout << ast2text.getResult() << std::endl;
}