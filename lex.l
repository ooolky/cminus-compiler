%{
#include "parser.tab.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

extern "C" int yylex();
%}

%option yylineno  noyywrap

id   	[A-Za-z_][A-Za-z0-9_]*  
int    	[0-9]+
float  	[0-9]*\.[0-9]+|[0-9]+\.
char    '([ -&(-[\]-~]|(\\([abfnrtv\\?'"0]|([0-3][0-7][0-7])|(x[0-9a-fA-F][0-9a-fA-F]))))'
lcomment \/\/[^\n]*
bcomment \/\*([^\*]|(\*)*[^\*/])*(\*)*\*\/
invalid_char '([ -&(-[\]-~]|(\\([abfnrtv\\?'"0]|([0-3][0-7][0-7])|(x[0-9a-fA-F][0-9a-fA-F]))))[^']+(')?

%%
{lcomment} {   printf("(LCOMMENT, %s)\n", yytext);    }

{bcomment} {   printf("(BCOMMENT, %s)\n", yytext);   }

{int}	{
    printf("(INT, %s)\n", yytext);
    yylval.type_int=atoi(yytext);
    return INT;
}

{float} {
    printf("(FLOAT, %s)\n", yytext);
    yylval.type_float=atof(yytext);
    return FLOAT; 
}

{char} {
    printf("(CHAR, %s)\n", yytext);
    yytext[strlen(yytext)-1]=0; 
    yylval.type_char=atol(yytext+1);
    return CHAR; 
}

"int" {
    printf("(TYPE, %s)\n", yytext);
    strcpy(yylval.type_id, yytext); 
    return TYPE; 
}

"float"	{
    printf("(TYPE, %s)\n", yytext);
    strcpy(yylval.type_id, yytext); 
    return TYPE; 
}

"char" {
    printf("(TYPE, %s)\n", yytext);
    strcpy(yylval.type_id, yytext); 
    return TYPE; 
}

"void" {
    printf("(TYPE, %s)\n", yytext);
    strcpy(yylval.type_id, yytext); 
    return TYPE; 
}

"return" {
    printf("(RETURN, -)\n");
    return RETURN; 
}

"if" {
    printf("(IF, -)\n");
    return IF; 
}

"else" { 
    printf("(ELSE, -)\n"); 
    return ELSE; 
}

"while" {   
    printf("(WHILE, -)\n");    
    return WHILE; 
}

"continue" {  
    printf("(CONTINUE, -)\n");    
    return CONTINUE; 
}

"break" {  
    printf("(BREAK, -)\n");   
    return BREAK; 
}

{id} {   
    printf("(ID, %s)\n", yytext);  
    strncpy(yylval.type_id, yytext, 31); 
    return ID; 
}

";"	{  
    printf("(SEMI, -)\n");   
    return SEMI; 
}

","	{   
    printf("(COMMA, -)\n");    
    return COMMA; 
}

"=="|"!="|">="|"<="|">"|"<"	{  
    printf("(RELOP, %s)\n", yytext);   
    strcpy(yylval.type_id, yytext); 
    return RELOP; 
}

"+="|"-="|"*="|"/="|"%="|"&="|"|="|"^="|"<<="|">>=" {  
    printf("(COMP_ASSIGN, %s)\n", yytext);   
    strcpy(yylval.type_id, yytext); 
    return COMP_ASSIGN; 
}

"="	{ 
     printf("(ASSIGNOP, -)\n");   
    return ASSIGNOP; 
}

"+"	{   
    printf("(PLUS, -)\n");  
    return PLUS; 
}

"-"	{   
    printf("(MINUS, -)\n");  
    return MINUS; 
}

"*"	{  
    printf("(STAR, -)\n");   
    return STAR; 
}

"/"	{   
    printf("(DIV, -)\n");  
    return DIV; 
}

"%"	{   
   printf("(MOD, -)\n");   
    return MOD; 
}

"&" { 
     printf("(BITAND, -)\n");   
    return BITAND; 
}

"|" {   
    printf("(BITOR, -)\n");  
    return BITOR; 
}

"^" {   
    printf("(BITXOR, -)\n");   
    return BITXOR; 
}

"<<" {  
    printf("(BITSHL, -)\n"); 
    return BITSHL; 
}

">>" { 
    printf("(BITSHR, -)\n"); 
    return BITSHR; 
}

"++" {  
    printf("(DPLUS, -)\n"); 
    return DPLUS; 
}

"--" { 
    printf("(DMINUS, -)\n"); 
    return DMINUS; 
}

"&&" { 
    printf("(AND, -)\n"); 
    return AND; 
}

"||" { 
    printf("(OR, -)\n"); 
    return OR; 
}

"!"	{ 
    printf("(NOT, -)\n"); 
    return NOT; 
}

"("	{ 
    printf("(LP, -)\n"); 
    return LP; 
}

")"	{  
    printf("(RP, -)\n"); 
    return RP; 
}

"{"	{ 
    printf("(LC, -)\n");  
    return LC; 
}

"}"	{ 
    printf("(RC, -)\n"); 
    return RC; 
}

"[" { 
    printf("(LS, -)\n");  
    return LS; 
}

"]" {  
    printf("(RS, -)\n");   
    return RS; 
}

[ \r\n\t]   {}

({int}|{float}|({int}|{float}))[\.a-zA-Z_][\.a-zA-Z_0-9]* { 
    printf("Lex Error: invalid identifier \"%s\" at line %d\n", yytext, yylineno); 
}

{invalid_char} { printf("Lex Error: invalid character constant %s at line %d\n", yytext, yylineno); return CHAR; }
.   { printf("Lex Error: mysterious character \"%s\" at line %d\n", yytext, yylineno); }
