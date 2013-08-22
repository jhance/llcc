%{
#include <string.h>
#include <stdlib.h>

#include "syntax.h"
#include "list.h"
#include "module_body.h"
#include "parser.h"
%}

%option noyywrap

%%

","                     { return TOKEN_COMMA; }
";"                     { return TOKEN_SEMICOLON; }
"{"                     { return TOKEN_LBRACE; }
"}"                     { return TOKEN_RBRACE; }
"("                     { return TOKEN_LPAREN; }
")"                     { return TOKEN_RPAREN; }
"="                     { return TOKEN_ASSIGN; }
"int"                   { return TOKEN_INT;    }
"char"                  { return TOKEN_CHAR;   }
"struct"                { return TOKEN_STRUCT; }
[a-zA-Z][a-zA-Z0-9]*    { yylval.string_data = strdup(yytext); return TOKEN_IDENTIFIER; }
[0-9]+                  { yylval.int_data = atoi(yytext); return TOKEN_LITERAL_INT; }
[ \t\n]+ /* ignore */

%%
#ifndef yywrap
int yywrap() { return 1; }
#endif
