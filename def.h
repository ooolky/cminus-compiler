#ifndef __DEF_H__
#define __DEF_H__
#include "parser.tab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

struct Opn {
	union {
		int place;
		int const_int;
		float const_float;
		char label[8];
	};
	int kind;
};

struct TACNode {
	struct Opn opn1, opn2, result;
	struct TACNode *next, *prior;
	int op;
};

struct ASTNode {
	union {
		char type_id[32];                   //由标识符生成的叶结点
		int type_int;                       //由整常数生成的叶结点
		float type_float;                   //由浮点常数生成的叶结点
	};
    struct ASTNode *ptr[3];
	char Etrue[8], Efalse[8];				//中间代码，转移目标label
	char Snext[8];							//中间代码
	struct TACNode *code;					//中间代码链表头指针
	int kind;						
	int pos;								//结点行号，用于输出错误信息
	int type;								//语义分析，类型分析/检查
	int place;								//中间代码，符号表位置序号
};

struct Symbol {
	char name[32];
	char alias[8];
	int lev;
	int param;
	int type;
	int offset;
	int link;
	char flag;
};

struct ASTNode *mknode(int num, int kind, int pos, ...);
void display(struct ASTNode *T, int indent);

void analysis(struct ASTNode *T);
#endif