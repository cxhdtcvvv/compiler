#include <fstream>

#include "parser_AST.hpp"
#include "C_Parser.tab.hpp"


VALUE *g_ast;

int main()
{ 
    std::ofstream ofile;
    ofile.open("output.s");
    yyparse();
    
    VALUE* ast=g_ast;
    
    ast->Print(ofile);

    return 0; 
}
