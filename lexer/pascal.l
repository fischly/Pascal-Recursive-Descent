%{
    /* Lexer for pascal source files */
    /* Author: Felix Mitterer */

    extern "C" int yylex();

    #include <iostream>
    #include "../common/token-enum.h"

    #define PRINT(lexem)  (std::cout << "[Token: \"" << lexem << "\"]" << std::endl)
    #define PRINTC(comment, lexem) (std::cout << "[" << comment << ": \"" << lexem << "\"]" << std::endl)

    #define PRINT_ERROR_SYMBOL(lexem) (std::cerr << "ERROR: reading symbol \"" << lexem << "\" on line " << yylineno << std::endl)
    #define PRINT_ERROR_IDENTIFIER(lexem) (std::cerr << "ERROR: not allowed identifier  \"" << lexem << "\" on line " << yylineno << " (identifiers starting with digits are not allowed)" << std::endl)
%}

%option yylineno

whitespace  [ \t]
newline     "\n"
digit       [0-9]

integer     {digit}+
real        {digit}+(\.{digit}+)?
string      "'"[^']*"'"

identifier  [a-zA-Z_][a-zA-Z0-9_]*
wrong_identifier [0-9][a-zA-Z0-9_]*

%%
 /* --- Comments --- */
"{"[^}]*"}" { PRINTC("Ignoring comment", yytext); }

 /* --- Keywords --- */
program     { return TokenType::PROGRAM; }
function    { return TokenType::FUNCTION; }
procedure   { return TokenType::PROCEDURE; }
begin       { return TokenType::BEGIN_; }
end         { return TokenType::END_; }


while       { return TokenType::WHILE; }
do          { return TokenType::DO; }

if          { return TokenType::IF; }
then        { return TokenType::THEN; }
else        { return TokenType::ELSE; }

var         { return TokenType::VAR; }
of          { return TokenType::OF; }

 /* --- Syntactial token --- */
","         { return TokenType::COMMA; }
":"         { return TokenType::COLON; }
";"         { return TokenType::SEMICOLON; }
"."         { return TokenType::DOT; }
".."        { return TokenType::RANGE_DOTS; }
"("         { return TokenType::BRACKETS_OPEN; }
")"         { return TokenType::BRACKETS_CLOSING; }
"["         { return TokenType::SQUARE_OPEN; }
"]"         { return TokenType::SQUARE_CLOSING; }

 /* --- Data type leywords --- */
integer     { return TokenType::INTEGER; }
real        { return TokenType::REAL; }
boolean     { return TokenType::BOOLEAN; }
array       { return TokenType::ARRAY; }

 /* --- Operators --- */
":="        { return TokenType::OP_ASSIGNMENT; }
not         { return TokenType::OP_NOT; }

"="         { return TokenType::OP_EQUALS; }
"<>"        { return TokenType::OP_NOT_EQUALS; }
"<"         { return TokenType::OP_LESS;  }
"<="        { return TokenType::OP_LESS_EQUAL; }
">"         { return TokenType::OP_GREATER;  }
">="        { return TokenType::OP_GREATER_EQUAL; }

 /* Arithmetic operators */
"+"         { return TokenType::OP_ADD; }
"-"         { return TokenType::OP_SUB; }
"*"         { return TokenType::OP_MUL; }
"/"         { return TokenType::OP_DIV; }
div         { return TokenType::OP_INTEGER_DIV; }
 /* Logic operators */
and         { return TokenType::OP_AND; }
or          { return TokenType::OP_OR; }


 /* Literals */
{integer}           { return TokenType::LITERAL_INTEGER; }
{real}              { return TokenType::LITERAL_REAL; }
{string}            { return TokenType::LITERAL_STRING; }

 /* Identifier */
{identifier}        { return TokenType::IDENTIFIER; }

{newline}           { }  /* consume new lines */ 
{whitespace}        { }  /* consume all remaining whitespaces */

 /* Catching errors */
{wrong_identifier}  { PRINT_ERROR_IDENTIFIER(yytext); } /* identifiers starting with a digit */
.                   { PRINT_ERROR_SYMBOL(yytext); } /* everything else that was not recognized yet, must be erroneous */

%%


