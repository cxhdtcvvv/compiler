CPPFLAGS += -W -Wall -g
all: compiler

compiler: C_Parser.tab.cpp C_Lexer.yy.cpp compiler.cpp
	g++ $(CPPFLAGS) compiler.cpp C_Parser.tab.cpp C_Lexer.yy.cpp -o compiler

C_Lexer.yy.cpp: C_Lexer.l C_Parser.tab.hpp
	flex -o C_Lexer.yy.cpp C_Lexer.l

C_Parser.tab.cpp C_Parser.tab.hpp: C_Parser.y
	bison -d C_Parser.y -o C_Parser.tab.cpp

clean:
	rm -f lex.yy.cpp C_Parser.tab.cpp C_Parser.tab.hpp C_Lexer.yy.cpp compiler
