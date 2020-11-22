%{
#include "parser.tab.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

extern "C" int yylex();
%}

%option yylineno  noyywrap

id   	[A-Za-z_][A-Za-z0-9_]*  
int    	[0-9]+
hex     0[xX][0-9a-fA-F]+
float  	[0-9]*\.[0-9]+|[0-9]+\.
expo    [Ee][+-]?[0-9]+
char    '([ -&(-[\]-~]|(\\([abfnrtv\\?'"0]|([0-3][0-7][0-7])|(x[0-9a-fA-F][0-9a-fA-F]))))'
lcomment \/\/[^\n]*
bcomment \/\*([^\*]|(\*)*[^\*/])*(\*)*\*\/
invalid_char '([ -&(-[\]-~]|(\\([abfnrtv\\?'"0]|([0-3][0-7][0-7])|(x[0-9a-fA-F][0-9a-fA-F]))))[^']+(')?

%%
{lcomment} {
    #ifdef DEBUG
    printf("(LCOMMENT, %s)\n", yytext);
    #endif
}

{bcomment} {
    #ifdef DEBUG
    printf("(BCOMMENT, %s)\n", yytext);
    #endif
}

{int}|{hex}	{
    #ifdef DEBUG
    printf("(INT, %s)\n", yytext);
    #endif
    yylval.type_int=atoi_(yytext);
    return INT;
}

{float}|(({int}|{float}){expo}) {
    #ifdef DEBUG
    printf("(FLOAT, %s)\n", yytext);
    #endif
    yylval.type_float=atof(yytext);
    return FLOAT; 
}

{char} {
    #ifdef DEBUG
    printf("(CHAR, %s)\n", yytext);
    #endif
    yytext[strlen(yytext)-1]=0; 
    yylval.type_char=atoc(yytext+1);
    return CHAR; 
}

"int" {
    #ifdef DEBUG
    printf("(TYPE, %s)\n", yytext);
    #endif
    strcpy(yylval.type_id, yytext); 
    return TYPE; 
}

"float"	{
    #ifdef DEBUG
    printf("(TYPE, %s)\n", yytext);
    #endif
    strcpy(yylval.type_id, yytext); 
    return TYPE; 
}

"char" {
    #ifdef DEBUG
    printf("(TYPE, %s)\n", yytext);
    #endif
    strcpy(yylval.type_id, yytext); 
    return TYPE; 
}

"void" {
    #ifdef DEBUG
    printf("(TYPE, %s)\n", yytext);
    #endif
    strcpy(yylval.type_id, yytext); 
    return TYPE; 
}

"return" {
    #ifdef DEBUG
    printf("(RETURN, -)\n");
    #endif
    return RETURN; 
}

"if" {
    #ifdef DEBUG
    printf("(IF, -)\n");
    #endif
    return IF; 
}

"else" { 
    #ifdef DEBUG
    printf("(ELSE, -)\n"); 
    #endif
    return ELSE; 
}

"while" {
    #ifdef DEBUG
    printf("(WHILE, -)\n"); 
    #endif
    return WHILE; 
}

"continue" {
    #ifdef DEBUG
    printf("(CONTINUE, -)\n"); 
    #endif
    return CONTINUE; 
}

"break" {
    #ifdef DEBUG
    printf("(BREAK, -)\n"); 
    #endif
    return BREAK; 
}

{id} { 
    #ifdef DEBUG
    printf("(ID, %s)\n", yytext); 
    #endif
    strncpy(yylval.type_id, yytext, 31); 
    return ID; 
}

";"	{
    #ifdef DEBUG
    printf("(SEMI, -)\n"); 
    #endif
    return SEMI; 
}

","	{ 
    #ifdef DEBUG
    printf("(COMMA, -)\n"); 
    #endif
    return COMMA; 
}

"=="|"!="|">="|"<="|">"|"<"	{
    #ifdef DEBUG
    printf("(RELOP, %s)\n", yytext); 
    #endif
    strcpy(yylval.type_id, yytext); 
    return RELOP; 
}

"+="|"-="|"*="|"/="|"%="|"&="|"|="|"^="|"<<="|">>=" {
    #ifdef DEBUG
    printf("(COMP_ASSIGN, %s)\n", yytext); 
    #endif
    strcpy(yylval.type_id, yytext); 
    return COMP_ASSIGN; 
}

"="	{ 
    #ifdef DEBUG
    printf("(ASSIGNOP, -)\n"); 
    #endif
    return ASSIGNOP; 
}

"+"	{ 
    #ifdef DEBUG
    printf("(PLUS, -)\n"); 
    #endif
    return PLUS; 
}

"-"	{
    #ifdef DEBUG 
    printf("(MINUS, -)\n"); 
    #endif
    return MINUS; 
}

"*"	{
    #ifdef DEBUG 
    printf("(STAR, -)\n"); 
    #endif
    return STAR; 
}

"/"	{ 
    #ifdef DEBUG
    printf("(DIV, -)\n"); 
    #endif
    return DIV; 
}

"%"	{ 
    #ifdef DEBUG
    printf("(MOD, -)\n");
    #endif 
    return MOD; 
}

"&" { 
    #ifdef DEBUG
    printf("(BITAND, -)\n"); 
    #endif
    return BITAND; 
}

"|" { 
    #ifdef DEBUG
    printf("(BITOR, -)\n"); 
    #endif
    return BITOR; 
}

"^" { 
    #ifdef DEBUG
    printf("(BITXOR, -)\n"); 
    #endif
    return BITXOR; 
}

"<<" { 
    #ifdef DEBUG
    printf("(BITSHL, -)\n"); 
    #endif
    return BITSHL; 
}

">>" { 
    #ifdef DEBUG
    printf("(BITSHR, -)\n"); 
    #endif
    return BITSHR; 
}

"++" { 
    #ifdef DEBUG
    printf("(DPLUS, -)\n"); 
    #endif
    return DPLUS; 
}

"--" { 
    #ifdef DEBUG
    printf("(DMINUS, -)\n"); 
    #endif
    return DMINUS; 
}

"&&" { 
    #ifdef DEBUG
    printf("(AND, -)\n"); 
    #endif
    return AND; 
}

"||" { 
    #ifdef DEBUG
    printf("(OR, -)\n"); 
    #endif
    return OR; 
}

"!"	{ 
    #ifdef DEBUG
    printf("(NOT, -)\n"); 
    #endif
    return NOT; 
}

"("	{ 
    #ifdef DEBUG
    printf("(LP, -)\n"); 
    #endif
    return LP; 
}

")"	{
    #ifdef DEBUG 
    printf("(RP, -)\n"); 
    #endif
    return RP; 
}

"{"	{ 
    #ifdef DEBUG
    printf("(LC, -)\n"); 
    #endif
    return LC; 
}

"}"	{ 
    #ifdef DEBUG
    printf("(RC, -)\n"); 
    #endif
    return RC; 
}

"[" { 
    #ifdef DEBUG
    printf("(LS, -)\n"); 
    #endif
    return LS; 
}

"]" { 
    #ifdef DEBUG
    printf("(RS, -)\n"); 
    #endif
    return RS; 
}

[ \r\n\t]   {}

({int}|{hex}|{float}|(({int}|{float}){expo}))[\.a-zA-Z_][\.a-zA-Z_0-9]* { 
    printf("Lex Error: invalid identifier \"%s\" at line %d\n", yytext, yylineno); 
}

{invalid_char} { printf("Lex Error: invalid character constant %s at line %d\n", yytext, yylineno); return CHAR; }
.   { printf("Lex Error: mysterious character \"%s\" at line %d\n", yytext, yylineno); }
