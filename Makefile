
testlexer:
	flex -o lexer/lex.yy.c lexer/pascal.l
	g++ -g -o pascal-parser parser/Parser.cpp -lfl
	cat test-code/$(file) | ./pascal-parser


clean: 
	rm lexer/lex.yy.c pascal-parser
