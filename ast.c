#include "def.h"

struct ASTNode *mknode(int num, int kind, int pos, ...) {
    struct ASTNode *T = (struct ASTNode *)calloc(1, sizeof(struct ASTNode));
    int i;
    T->kind = kind;
    T->pos = pos;
    va_list pArgs;
    va_start(pArgs, pos);
    for(i=0; i<num; i++)
        T->ptr[i] = va_arg(pArgs, struct ASTNode *);
    while (i < 3) T->ptr[i++] = NULL;
    va_end(pArgs);
    return T;
}

void display(struct ASTNode *T,int indent) {
    if (T) {
	    switch (T->kind) {
	    case EXT_DEF_LIST:  
            display(T->ptr[0], indent);         //EXT_VAR_DEF|FUNC_DEF
            display(T->ptr[1], indent);         //EXT_DEF_LIST
            break;
	    case TYPE:          
            printf("%*cTYPE：%s\n", indent, ' ', T->type_id);
            break;
        case EXT_VAR_DEF:
            printf("%*cEXT_VAR_DEF：(%d)\n", indent, ' ', T->pos);
            display(T->ptr[0], indent+3);       //TYPE
            printf("%*cVAR_NAME：\n", indent+3, ' ');
            display(T->ptr[1], indent+6);       //EXT_DEC_LIST
            break;
        case EXT_DEC_LIST:  
            display(T->ptr[0], indent);         //VAR_DEC|ARRAY_DEC
            display(T->ptr[1], indent);         //EXT_DEC_LIST
            break;
        case VAR_DEC:
            if (T->ptr[0]) {
                printf("%*c%s ASSIGNOP\n", indent, ' ', T->type_id);
                display(T->ptr[0], indent+3);   //EXP
            } else {
                printf("%*c%s\n", indent, ' ', T->type_id);
            }
            break;
	    case FUNC_DEF:
            printf("%*cFUNC_DEF：(%d)\n", indent, ' ', T->pos);
            display(T->ptr[0], indent+3);       //TYPE
            display(T->ptr[1], indent+3);       //FUNC_DEC
            display(T->ptr[2], indent+3);       //COMP_STM
            break;
	    case FUNC_DEC:      
            printf("%*cFUNC_NAME：%s\n", indent, ' ', T->type_id);
            if (T->ptr[0]) {
                printf("%*cPARAM_LIST：\n", indent, ' ');
                display(T->ptr[0], indent+3);   //PARAM_LIST
            } else {
                printf("%*cNO_PARAM\n", indent+3, ' ');
            }
            break;
	    case PARAM_LIST:    
            display(T->ptr[0], indent);         //PARAM_DEC|ARRAY_PARAM
            display(T->ptr[1], indent);         //PARAM_LIST
            break;
	    case PARAM_DEC:          
            printf("%*cNAME:%s,TYPE:%s\n", indent, ' ', T->type_id, T->ptr[0]->type_id);
            break;
        case COMP_STM:      
            printf("%*cCOMP_STM：(%d)\n", indent, ' ', T->pos);
            display(T->ptr[0], indent+3);       //STM_LIST
            break;
        case STM_LIST:      
            display(T->ptr[0], indent);         //EXP_STMT, RETURN...
            display(T->ptr[1], indent);         //STM_LIST
            break;
        case VAR_DEF:       
            printf("%*cVAR_DEF：(%d)\n", indent, ' ', T->pos);
            display(T->ptr[0], indent+3);       //TYPE
            printf("%*cVAR_NAME：\n", indent+3, ' ');
            display(T->ptr[1], indent+6);       //VAR_DEC_LIST
            break;
        case VAR_DEC_LIST:
            display(T->ptr[0], indent);         //VAR_DEC
            display(T->ptr[1], indent);         //VAR_DEC_LIST
            break;
	    case EXP_STMT:      
            printf("%*cEXP_STMT：(%d)\n", indent, ' ', T->pos);
            display(T->ptr[0], indent+3);       //EXP
            break;
	    case RETURN:        
            printf("%*cRETURN：(%d)\n", indent, ' ', T->pos);
            display(T->ptr[0], indent+3);       //EXP
            break;
	    case IF_THEN:       
            printf("%*cIF(IF_THEN)：(%d)\n", indent, ' ', T->pos);
            printf("%*cCONDITION：\n", indent+3, ' ');
            display(T->ptr[0], indent+6);       //EXP
            printf("%*cIF_STMT：(%d)\n",indent+3,' ',T->pos);
            display(T->ptr[1], indent+6);       //STMT
            break;
	    case IF_THEN_ELSE:  
            printf("%*cIF(IF_THEN_ELSE)：(%d)\n", indent, ' ', T->pos);
            printf("%*cCONDITION：\n", indent+3, ' ');
            display(T->ptr[0], indent+6);       //EXP
            printf("%*cIF_STMT：(%d)\n", indent+3, ' ', T->pos);
            display(T->ptr[1], indent+6);       //STMT
            printf("%*cELSE_STMT：(%d)\n", indent+3, ' ', T->pos);
            display(T->ptr[2], indent+6);       //STMT
            break;
	    case WHILE:         
            printf("%*cWHILE：(%d)\n",indent, ' ', T->pos);
            printf("%*cCONDITION：\n", indent+3, ' ');
            display(T->ptr[0], indent+6);       //EXP
            printf("%*cSTMT：(%d)\n", indent+3, ' ', T->pos);
            display(T->ptr[1], indent+6);       //STMT
            break;
        case CONTINUE:
            printf("%*cCONTINUE(%d)\n", indent, ' ', T->pos);
            break;
        case BREAK:
            printf("%*cBREAK(%d)\n", indent, ' ', T->pos);
            break;
	    case ID:	        
            printf("%*c%s:ID\n", indent, ' ', T->type_id);
            break;
	    case INT:	        
            printf("%*c%d:INT\n", indent, ' ', T->type_int);
            break;
	    case FLOAT:	       
            printf("%*c%f:FLOAT\n", indent, ' ', T->type_float);
            break;
	    case ASSIGNOP:
            printf("%*cASSIGNOP\n", indent, ' ');
            display(T->ptr[0], indent+3);
            display(T->ptr[1], indent+3);
            break;
        case COMP_ASSIGN:
            printf("%*cCOMP_ASSIGN %s\n", indent, ' ', T->type_id);
            display(T->ptr[0], indent+3);
            display(T->ptr[1], indent+3);
            break;
	    case AND:
            printf("%*cAND\n", indent, ' ');
            display(T->ptr[0], indent+3);
            display(T->ptr[1], indent+3);
            break;
	    case OR:
            printf("%*cOR\n", indent, ' ');
            display(T->ptr[0], indent+3);
            display(T->ptr[1], indent+3);
            break;
	    case RELOP:
            printf("%*cRELOP %s\n", indent, ' ', T->type_id);
            display(T->ptr[0], indent+3);
            display(T->ptr[1], indent+3);
            break;
	    case PLUS:
            printf("%*cPLUS\n", indent, ' ');
            display(T->ptr[0], indent+3);
            display(T->ptr[1], indent+3);
            break;
	    case MINUS:
            printf("%*cMINUS\n", indent, ' ');
            display(T->ptr[0], indent+3);
            display(T->ptr[1], indent+3);
            break;
	    case STAR:
            printf("%*cMULT\n", indent, ' ');
            display(T->ptr[0], indent+3);
            display(T->ptr[1], indent+3);
            break;
	    case DIV:
            printf("%*cDIV\n", indent, ' ');
            display(T->ptr[0], indent+3);
            display(T->ptr[1], indent+3);
            break;
        case MOD:
            printf("%*cMOD\n", indent, ' ');
            display(T->ptr[0], indent+3);
            display(T->ptr[1], indent+3);
            break;
        case BITAND:
            printf("%*cBITAND\n", indent, ' ');
            display(T->ptr[0], indent+3);
            display(T->ptr[1], indent+3);
            break;
        case BITOR:
            printf("%*cBITOR\n", indent, ' ');
            display(T->ptr[0], indent+3);
            display(T->ptr[1], indent+3);
            break;
        case BITXOR:
            printf("%*cBITXOR\n", indent, ' ');
            display(T->ptr[0], indent+3);
            display(T->ptr[1], indent+3);
            break;
        case BITSHL:
            printf("%*cBITSHL\n", indent, ' ');
            display(T->ptr[0], indent+3);
            display(T->ptr[1], indent+3);
            break;
        case BITSHR:
            printf("%*cBITSHR\n", indent, ' ');
            display(T->ptr[0], indent+3);
            display(T->ptr[1], indent+3);
            break;
	    case NOT:
            printf("%*cNOT\n", indent, ' ');
            display(T->ptr[0], indent+3);
            break;
	    case UMINUS:
            printf("%*cUMINUS\n", indent, ' ');
            display(T->ptr[0], indent+3);
            break;
        case DPLUS:
        case DMINUS:
            printf("%*c%s\n", indent, ' ', T->type_id);
            display(T->ptr[0], indent+3);
            break;
        case FUNC_CALL: 
            printf("%*cFUNC_CALL：%s(%d)\n", indent, ' ', T->type_id, T->pos);
            if (T->ptr[0]) {
                printf("%*cPARAM_LIST：\n", indent+3, ' ');
                display(T->ptr[0], indent+6);   //ARGS
            } else {
                printf("%*cNO_PARAM\n", indent+6, ' ');
            }
            break;
	    case ARGS:
            display(T->ptr[0], indent);         //EXP
            display(T->ptr[1], indent);         //ARGS
            break;
        case ARRAY_DEC:
            printf("%*cARRAY_DEC:%s\n", indent, ' ', T->type_id);
            printf("%*cARRAY_SUB_LIST:\n", indent+3, ' ');
            display(T->ptr[0], indent+6);       //ARRAY_SUB_LIST
            display(T->ptr[1], indent+3);   
            break;
        case ARRAY_SUB_LIST:
            if (T->ptr[0] == NULL) {
                printf("%*cNO EXP\n", indent, ' ');
                display(T->ptr[1], indent);     //ARRAY_SUB_LIST
            } else {
                display(T->ptr[0], indent);     //EXP
                display(T->ptr[1], indent);     //ARRAY_SUB_LIST
            }
            break;
        case ARRAY_REF:
            printf("%*cARRAY_REF：%s\n", indent, ' ', T->type_id);
            printf("%*cARRAY_SUB_LIST:\n", indent+3, ' ');
            display(T->ptr[0], indent+6);
            break;
        case ARRAY_INIT_LIST:
            printf("%*cARRAY_INIT_LIST:\n", indent, ' ');
            display(T->ptr[0], indent+3);
            break;
        case ARRAY_PARAM:
            printf("%*cARRAY_PARAM:\n", indent, ' ');          
            printf("%*cNAME:%s,TYPE:%s\n", indent+3, ' ', T->type_id, T->ptr[0]->type_id);
            printf("%*cARRAY_SUB_LIST:\n", indent+3, ' '); 
            display(T->ptr[1], indent+6);
            break;
        }
    }
}