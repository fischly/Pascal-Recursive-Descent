
testlexer:
	flex -o lexer/lex.yy.c lexer/pascal.l
	g++ -g -o pascal-parser.exe parser/Parser.cpp -lfl
	cat test-code/$(file) | ./pascal-parser.exe 


clean: 
	rm lexer/lex.yy.c pascal-parser.exe
