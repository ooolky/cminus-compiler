%define parse.error verbose
%locations
%{
#include "def.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

extern int yylineno;
extern char *yytext;
extern FILE *yyin;
extern "C" int yylex();
void yyerror(const char* fmt, ...);

extern int hasError;
%}

%union {
    int    type_int;
    float  type_float;
    char   type_char;
    char   type_id[32];
    struct ASTNode *ptr;
};

//%type 定义非终结符的语义值类型
%type  <ptr> Program ExtDefList ExtDef Specifier ExtDecList FuncDec CompSt ParamList VarDec ParamDec Stmt StmList VarDecList Exp Args
%type  <ptr> ArrayDec ArraySubList ArrayInitList DimensionList

//%token 定义终结符的语义值类型
%token <type_int> INT                       
%token <type_id> ID RELOP TYPE COMP_ASSIGN  
%token <type_float> FLOAT                   
%token <type_char> CHAR

%token LP RP LC RC LS RS SEMI COMMA LB RB        
%token PLUS MINUS STAR DIV MOD ASSIGNOP AND OR NOT
%token BITAND BITOR BITXOR BITSHL BITSHR
%token IF ELSE WHILE RETURN CONTINUE BREAK
%token EXT_DEF_LIST EXT_VAR_DEF FUNC_DEF FUNC_DEC EXT_DEC_LIST PARAM_LIST PARAM_DEC VAR_DEF VAR_DEC VAR_DEC_LIST COMP_STM STM_LIST EXP_STMT IF_THEN IF_THEN_ELSE
%token FUNC_CALL ARGS ARRAY_DEC ARRAY_REF ARRAY_SUB_LIST ARRAY_INIT_LIST ARRAY_PARAM DIMENSION DIMENSION_LIST
%token VAR PARAM FUNC ARG VOID ARRAY 
%token LABEL GOTO
%token EQ NEQ LT LE GT GE

%right ASSIGNOP COMP_ASSIGN
%left OR
%left AND
%left BITOR
%left BITXOR
%left BITAND
%left RELOP
%left BITSHL BITSHR
%left PLUS MINUS
%left STAR DIV MOD
%right NOT UMINUS DPLUS DMINUS

%nonassoc LOWER_THEN_ELSE
%nonassoc ELSE

%%

Program: ExtDefList {
    display($1, 0);
    analysis($1);
    if (hasError) {
        fprintf(stderr, "Detect fatal errors, compiler terminated!\n");
        exit(-1);
    }
}                           
;

ExtDefList: { $$ = NULL; }
| ExtDef ExtDefList { $$ = mknode(2, EXT_DEF_LIST, yylineno, $1, $2); }              
;

ExtDef: Specifier ExtDecList SEMI { $$ = mknode(2, EXT_VAR_DEF, yylineno, $1, $2); }   
| Specifier FuncDec CompSt { $$ = mknode(3, FUNC_DEF, yylineno, $1, $2, $3); }       
| error SEMI { $$ = NULL; }
;

Specifier: TYPE { $$ = mknode(0, TYPE, yylineno); strcpy($$->type_id, $1);}   
;

ExtDecList: VarDec { $$ = mknode(1, EXT_DEC_LIST, yylineno, $1); }      
| VarDec COMMA ExtDecList { $$ = mknode(2, EXT_DEC_LIST, yylineno, $1, $3); }
| ArrayDec { $$ = mknode(1, EXT_DEC_LIST, yylineno, $1); }
| ArrayDec COMMA ExtDecList { $$ = mknode(2, EXT_DEC_LIST, yylineno, $1, $3); }
;

VarDec: ID { $$ = mknode(0, VAR_DEC, yylineno); strcpy($$->type_id, $1); }
| ID ASSIGNOP Exp { $$ = mknode(1, VAR_DEC, yylineno, $3); strcpy($$->type_id, $1); }
| ID DimensionList {$$=mknode(1,DIMENSION,yylineno,$2);strcpy($$->type_id,$1);}
;

FuncDec: ID LP ParamList RP { $$ = mknode(1, FUNC_DEC, yylineno, $3); strcpy($$->type_id, $1); }  
| ID LP RP { $$ = mknode(0, FUNC_DEC, yylineno); strcpy($$->type_id, $1); $$->ptr[0] = NULL; }  
;

ParamList: ParamDec { $$ = mknode(1, PARAM_LIST, yylineno, $1); }
| ParamDec COMMA ParamList { $$ = mknode(2, PARAM_LIST, yylineno, $1, $3); }
;

ParamDec: Specifier ID { $$ = mknode(1, PARAM_DEC, yylineno, $1); strcpy($$->type_id, $2); }
| Specifier ID ArraySubList { $$ = mknode(2, ARRAY_PARAM, yylineno, $1, $3); strcpy($$->type_id, $2); }
;

CompSt: LC StmList RC { $$ = mknode(1, COMP_STM, yylineno, $2); }
;

StmList: { $$=NULL; }
| Stmt StmList { $$ = mknode(2, STM_LIST, yylineno, $1, $2); }
;

Stmt: Specifier VarDecList SEMI { $$ = mknode(2, VAR_DEF, yylineno, $1, $2); }
| Exp SEMI { $$ = mknode(1, EXP_STMT, yylineno, $1); }
| CompSt { $$=$1; }      //复合语句结点直接作为语句结点，不再生成新的结点
| RETURN Exp SEMI { $$ = mknode(1, RETURN, yylineno, $2); }
| RETURN SEMI { $$ = mknode(0, RETURN, yylineno); }
| IF LP Exp RP Stmt %prec LOWER_THEN_ELSE { $$ = mknode(2, IF_THEN, yylineno, $3, $5); }
| IF LP Exp RP Stmt ELSE Stmt { $$ = mknode(3, IF_THEN_ELSE, yylineno, $3, $5, $7); }
| WHILE LP Exp RP Stmt { $$ = mknode(2, WHILE, yylineno, $3, $5); }
| SEMI { $$ = NULL; }
| CONTINUE SEMI { $$ = mknode(0, CONTINUE, yylineno); }
| BREAK SEMI { $$ = mknode(0, BREAK, yylineno); }
;

VarDecList: VarDec { $$ = mknode(1, VAR_DEC_LIST, yylineno, $1); }
| VarDec COMMA VarDecList { $$ = mknode(2, VAR_DEC_LIST, yylineno, $1, $3); }
| ArrayDec { $$ = mknode(1, VAR_DEC_LIST, yylineno, $1); }
| ArrayDec COMMA ExtDecList { $$ = mknode(2, VAR_DEC_LIST, yylineno, $1, $3); }
;

Exp: Exp ASSIGNOP Exp { $$ = mknode(2, ASSIGNOP, yylineno, $1, $3); }
| Exp AND Exp { $$ = mknode(2, AND, yylineno, $1, $3); }
| Exp OR Exp { $$ = mknode(2, OR, yylineno, $1, $3); }
| Exp RELOP Exp { $$ = mknode(2, RELOP, yylineno, $1, $3); strcpy($$->type_id, $2); }  
| Exp PLUS Exp { $$ = mknode(2, PLUS, yylineno, $1, $3); }
| Exp MINUS Exp { $$ = mknode(2, MINUS, yylineno, $1, $3); }
| Exp STAR Exp { $$ = mknode(2, STAR, yylineno, $1, $3); }
| Exp DIV Exp { $$ = mknode(2, DIV, yylineno, $1, $3); }
| Exp MOD Exp { $$ = mknode(2, MOD, yylineno, $1, $3); }
| Exp COMP_ASSIGN Exp { $$ = mknode(2, COMP_ASSIGN, yylineno, $1, $3); strcpy($$->type_id, $2); }
| Exp BITAND Exp { $$ = mknode(2, BITAND, yylineno, $1, $3); }
| Exp BITOR Exp { $$ = mknode(2, BITOR, yylineno, $1, $3); }
| Exp BITXOR Exp { $$ = mknode(2, BITXOR, yylineno, $1, $3); }
| Exp BITSHL Exp { $$ = mknode(2, BITSHL, yylineno, $1, $3); }
| Exp BITSHR Exp { $$ = mknode(2, BITSHR, yylineno, $1, $3); }
| LP Exp RP { $$ = $2; }
| MINUS Exp %prec UMINUS { $$ = mknode(1, UMINUS, yylineno, $2); }
| NOT Exp { $$ = mknode(1, NOT, yylineno, $2); }
| DPLUS Exp { $$ = mknode(1, DPLUS, yylineno, $2); strcpy($$->type_id, "LDPLUS"); }
| Exp DPLUS { $$ = mknode(1, DPLUS, yylineno, $1); strcpy($$->type_id, "RDPLUS"); }
| DMINUS Exp { $$ = mknode(1, DMINUS, yylineno, $2); strcpy($$->type_id, "LDMINUS"); }
| Exp DMINUS { $$ = mknode(1, DMINUS, yylineno, $1); strcpy($$->type_id, "RDMINUS"); }
| ID LP Args RP { $$ = mknode(1, FUNC_CALL, yylineno, $3); strcpy($$->type_id, $1); }
| ID LP RP { $$ = mknode(0, FUNC_CALL, yylineno); strcpy($$->type_id, $1); }
| ID ArraySubList { $$ = mknode(1, ARRAY_REF, yylineno, $2); strcpy($$->type_id, $1); }
| ID { $$ = mknode(0, ID, yylineno); strcpy($$->type_id, $1); }
| INT { $$ = mknode(0, INT, yylineno); $$->type_int = $1; }
| FLOAT { $$ = mknode(0, FLOAT, yylineno); $$->type_float = $1; }
| CHAR { $$ = mknode(0, INT, yylineno); $$->type_int = $1; }
;

Args: Exp COMMA Args { $$ = mknode(2, ARGS, yylineno, $1, $3); }
| Exp { $$ = mknode(1, ARGS, yylineno, $1); }
;

ArrayDec: ID ArraySubList { $$ = mknode(1, ARRAY_DEC, yylineno, $2); strcpy($$->type_id, $1); }
| ID ArraySubList ASSIGNOP ArrayInitList { $$ = mknode(2, ARRAY_DEC, yylineno, $2, $4); strcpy($$->type_id, $1); }
;

ArraySubList: LS Exp RS { $$ = mknode(1, ARRAY_SUB_LIST, yylineno, $2); }
| LS Exp RS ArraySubList { $$ = mknode(2, ARRAY_SUB_LIST, yylineno, $2, $4); }
| LS RS { $$ = mknode(1, ARRAY_SUB_LIST, yylineno, NULL); }
| LS RS ArraySubList { $$ = mknode(2, ARRAY_SUB_LIST, yylineno, NULL, $3); }
;

ArrayInitList: LC Args RC { $$ = mknode(1, ARRAY_INIT_LIST, yylineno, $2); }
;

DimensionList:LB INT RB {$$=mknode(1,DIMENSION_LIST,yylineno,$2);$$->type_int=$2;}
 | LB INT RB DimensionList {$$=mknode(2,DIMENSION_LIST,yylineno,$2,$4);$$->type_int=$2;}
 ;
       
%%

int main(int argc, char *argv[]) {
    yyin = fopen(argv[1], "r");
    if (!yyin) return -1;
    yylineno = 1;
    yyparse();
    return 0;
}

void yyerror(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Syntax Error at line %d: ", yylineno);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
}