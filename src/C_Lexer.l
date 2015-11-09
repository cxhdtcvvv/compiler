%{
#include <iostream>
#include "C_Parser.tab.hpp"

using namespace std;
%}

int [0-9]+
double [0-9]+\.[0-9]+
id [a-zA-Z]+[0-9]*_?[a-zA-Z]*[0-9]*
str ["][^"]*["]

%%
"int main"      { return INTMAIN;}
"int"           { return TYPE_INT;}
"double"        { return TYPE_DOUBLE;}
"while"         { return WHILE;}
"if"            { return IF;}

{int}           { yylval.tok_int = atoi(yytext); return V_INT; }
{double}        { yylval.tok_double = atoi(yytext); return V_DOUBLE; }
{id}            { yylval.tok_string = strdup(yytext); return IDENTIFIER;}
{str}           { yylval.tok_string=  strdup(yytext); return STRING; }


"-"             { yylval.tok_char = yytext[0]; return MINOP; }
"+"             { yylval.tok_char = yytext[0]; return ADDOP; }
"*"             { yylval.tok_char = yytext[0]; return MULTOP; }
"/"             { yylval.tok_char = yytext[0]; return DIVOP; }

","		{ return COMMA; }
"="             { return ASSIGN; }
";"             { return SEMICOLM; }
"("             { return LBRACKET; }
")"             { return RBRACKET; }
"{"             { return LCURLYBRACKET; }
"}"             { return RCURLYBRACKET; }
[\s]+		{}
\n              {}

%%
int yywrap(void){
	return 1;
}







