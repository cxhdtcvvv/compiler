// in this example everything is passed around as a string 
// and converted to and from other types when needed

%code requires{

#include <iostream>

#include "parser_AST.hpp"

extern VALUE *g_ast;
 int yylex(void);
 void yyerror(char *);

}


%union {
    int tok_int;               
    double tok_double;
    char tok_char;
    const char* tok_string;          
    
    INTEGER* num_int;
    DOUBLE* num_double;
    OPERATOR* Op;
    ID* Var;
    
    FUNC_IMPL* Func_impl;
    FUNC_CALL* Func_call;
    FUNC_INPUT* Func_input;
    TYPE* Type;
    VALUE* Value;
    EXPR* Expr;
    IDENS_v* Idens_v;
    DECLARATION* Declaration;
    ASSIGNMENT* Assignment;
    FUNC_UNIT* fu;
    PROGRAM* Program;

    
};


%token<tok_string> IDENTIFIER STRING TYPE_INT TYPE_DOUBLE
%token<tok_int> V_INT
%token<tok_double> V_DOUBLE
%token<tok_char> MINOP ADDOP MULTOP DIVOP SEMICOLM 
%token LBRACKET RBRACKET INTMAIN LCURLYBRACKET RCURLYBRACKET WHILE IF COMMA
%token<tok_int> ASSIGN

%type<Func_call> func_call
%type<Assignment> assigment
%type<Declaration> declaration
%type<Func_impl> func_impl
%type<Func_input> func_input
%type<fu> func_impl_unit
%type<Program> program
%type<Expr> expr
%type<Idens_v> idens

%left ADDOP MINOP
%left MULTOP DIVOP

%start program


%{
struct pair_t *root=0;
std::vector<std::string> id_stack;

%}

%%


program:
INTMAIN LBRACKET RBRACKET LCURLYBRACKET func_impl RCURLYBRACKET   {g_ast = $5;}

;

func_impl:
func_impl_unit SEMICOLM 		{std::cout<<"new"<<std::endl; $$ = new FUNC_IMPL();$$->debug($1); $$->Add($1); $$->build_map();}
|func_impl func_impl_unit SEMICOLM 		{std::cout<<"new+"<<std::endl; $1->debug($2);$1->Add($2); $1->build_map(); $$ = $1;}
;

func_impl_unit :
declaration			{std::cout<<"Decla:"<<std::endl; $$ = new FUNC_UNIT(); $$->type =1; $$->decla = $1;}
| assigment			{std::cout<<"Assignment:"<<std::endl;$$ = new FUNC_UNIT();$$->type =2; $$->assign = $1;}
| func_call			{std::cout<<"Function call:"<<std::endl;$$ = new FUNC_UNIT(); $$->type =3; $$->funcall = $1;}
| IF LBRACKET expr RBRACKET LCURLYBRACKET func_impl RCURLYBRACKET {std::cout<<"If flow:"<<std::endl;}
| WHILE LBRACKET expr RBRACKET LCURLYBRACKET func_impl RCURLYBRACKET {}
;

func_call: 
IDENTIFIER LBRACKET func_input RBRACKET 		{ $$ = new FUNC_CALL($1, $3);}
;

func_input:	
expr 					{$$ = new FUNC_INPUT();  $$->Add($1);}
|func_input COMMA expr 				{$1->Add($3); $$ = $1; }
;

declaration:	
TYPE_INT IDENTIFIER ASSIGN expr 		{$$ = new DECLARATION();  std::string temp = "int";$$->Add_1(temp, $2, $4,1); }
| TYPE_DOUBLE IDENTIFIER ASSIGN expr 	{$$ = new DECLARATION();std::string temp ="double";$$->Add_1(temp, $2, $4,1); }
| TYPE_DOUBLE idens 		 	{$$ = new DECLARATION();std::string temp ="double";$$->Add_2(temp, $2,2); }
| TYPE_INT idens 		 		{$$ = new DECLARATION();std::string temp ="int";$$->Add_2(temp, $2,2);}
;

idens:
	  IDENTIFIER		{ $$ = new IDENS_v(); $$->Add($1);}
	| idens COMMA IDENTIFIER	{$1->Add($3);$$ =$1; }
;

assigment:
      IDENTIFIER ASSIGN expr			{$$ = new ASSIGNMENT($1,$3);}
;

expr:
 V_INT                 {std::cout<<"int:"<<std::endl; $$ = new EXPR();$$->type =1; $$->f_int = $1;}
| V_DOUBLE             {std::cout<<"double:"<<std::endl; $$ = new EXPR();$$->type =2; $$->f_double= $1;}
| IDENTIFIER            {std::cout<<"var: "<<std::endl; $$ = new EXPR();$$->type =3; $$->f_id= $1;}
| LBRACKET expr RBRACKET    {$$ = $2;}
| expr ADDOP expr
    {std::cout<<"ADD: "<<std::endl; $$ = new EXPR(); $$->type =4;EXPR* temp = new EXPR(); temp->type= 5;temp->f_op = $2; $$->Add($1,temp,$3);}
| expr MINOP expr
    {std::cout<<"MIN: "<<std::endl; $$ = new EXPR(); $$->type =4;EXPR* temp = new EXPR(); temp->type= 5;temp->f_op = $2; $$->Add($1,temp,$3);}
| expr MULTOP expr
    {std::cout<<"MUL: "<<std::endl; $$ = new EXPR(); $$->type =4;EXPR* temp = new EXPR(); temp->type= 5;temp->f_op = $2; $$->Add($1,temp,$3);}
| expr DIVOP expr
    {std::cout<<"DIV: "<<std::endl; $$ = new EXPR(); $$->type =4;EXPR* temp = new EXPR(); temp->type= 5;temp->f_op = $2; $$->Add($1,temp,$3);}
| STRING                {std::cout<<"str:"<<std::endl; $$ = new EXPR();$$->type =6; $$->f_string = $1;}
;


%%
void yyerror(char *s){
    std::cout<< "error::::::::::::::" << s  << std::endl;
}







//void va_start (va_list ap, paramN);    Name of the last named parameter in the function definition.
