#include "def.h"

#include <vector>
#include <map>
#include <string>

using namespace std;

struct TypeVal {
    int type;
    union {
        float type_float;
        int type_int;
    };
};

static int curLev = 0;
static int curType;
static int funcType;
static int isExtVar = 1;
static int isFuncDef = 0;
static int inLoop = 0;

int hasError = 0;

vector<Symbol *> symbolTab;
map<string, int> funcTab;
static vector<int> bTab;

void genTAC(ASTNode *T);
static void displayTAC(TACNode *head);

static void newAlias(char *alias) {
    static int cnt = 0;
    sprintf(alias, "V%d", cnt);
}

static TypeVal getTypeVal(ASTNode *lT, ASTNode *rT, int kind, char *relop) {
    TypeVal typeVal;
    switch (kind) {
    case RELOP:
        switch (relop[0]) {
        case '=':
            if (lT->type == FLOAT) {
                if (rT->type == FLOAT)
                    typeVal.type_int = (lT->type_float == rT->type_float);
                else
                    typeVal.type_int = (lT->type_float == rT->type_int);
            } else {
                if (rT->type == FLOAT)
                    typeVal.type_int = (lT->type_int == rT->type_float);
                else
                    typeVal.type_int = (lT->type_int == rT->type_int);
            }
            break;
        case '!':
            if (lT->type == FLOAT) {
                if (rT->type == FLOAT)
                    typeVal.type_int = (lT->type_float != rT->type_float);
                else
                    typeVal.type_int = (lT->type_float != rT->type_int);
            } else {
                if (rT->type == FLOAT)
                    typeVal.type_int = (lT->type_int != rT->type_float);
                else
                    typeVal.type_int = (lT->type_int != rT->type_int);
            }
            break;
        case '>':
            if (relop[1]) {
                if (lT->type == FLOAT) {
                    if (rT->type == FLOAT)
                        typeVal.type_int = (lT->type_float >= rT->type_float);
                    else
                        typeVal.type_int = (lT->type_float >= rT->type_int);
                } else {
                    if (rT->type == FLOAT)
                        typeVal.type_int = (lT->type_int >= rT->type_float);
                    else
                        typeVal.type_int = (lT->type_int >= rT->type_int);
                }
            } else {
                if (lT->type == FLOAT) {
                    if (rT->type == FLOAT)
                        typeVal.type_int = (lT->type_float > rT->type_float);
                    else
                        typeVal.type_int = (lT->type_float > rT->type_int);
                } else {
                    if (rT->type == FLOAT)
                        typeVal.type_int = (lT->type_int > rT->type_float);
                    else
                        typeVal.type_int = (lT->type_int > rT->type_int);
                }
            }
            break;
        case '<':
            if (relop[1]) {
                if (lT->type == FLOAT) {
                    if (rT->type == FLOAT)
                        typeVal.type_int = (lT->type_float <= rT->type_float);
                    else
                        typeVal.type_int = (lT->type_float <= rT->type_int);
                } else {
                    if (rT->type == FLOAT)
                        typeVal.type_int = (lT->type_int <= rT->type_float);
                    else
                        typeVal.type_int = (lT->type_int <= rT->type_int);
                }
            } else {
                if (lT->type == FLOAT) {
                    if (rT->type == FLOAT)
                        typeVal.type_int = (lT->type_float < rT->type_float);
                    else
                        typeVal.type_int = (lT->type_float < rT->type_int);
                } else {
                    if (rT->type == FLOAT)
                        typeVal.type_int = (lT->type_int < rT->type_float);
                    else
                        typeVal.type_int = (lT->type_int < rT->type_int);
                }
            }
            break;
        default: 
            break;
        }
        typeVal.type = INT;
        break;
    case PLUS:
        if (lT->type == FLOAT) {
            if (rT->type == FLOAT)
                typeVal.type_float = (lT->type_float + rT->type_float);
            else
                typeVal.type_float = (lT->type_float + rT->type_int);
            typeVal.type = FLOAT;
        } else {
            if (rT->type == FLOAT) {
                typeVal.type_float = (lT->type_int + rT->type_float);
                typeVal.type = FLOAT;
            }
            else {
                typeVal.type_int = (lT->type_int + rT->type_int);
                typeVal.type = INT;
            }
        }
        break;
    case MINUS:
        if (lT->type == FLOAT) {
            if (rT->type == FLOAT)
                typeVal.type_float = (lT->type_float - rT->type_float);
            else
                typeVal.type_float = (lT->type_float - rT->type_int);
            typeVal.type = FLOAT;
        } else {
            if (rT->type == FLOAT) {
                typeVal.type_float = (lT->type_int - rT->type_float);
                typeVal.type = FLOAT;
            }
            else {
                typeVal.type_int = (lT->type_int - rT->type_int);
                typeVal.type = INT;
            }
        }
        break;
    case STAR:
        if (lT->type == FLOAT) {
            if (rT->type == FLOAT)
                typeVal.type_float = (lT->type_float * rT->type_float);
            else
                typeVal.type_float = (lT->type_float * rT->type_int);
            typeVal.type = FLOAT;
        } else {
            if (rT->type == FLOAT) {
                typeVal.type_float = (lT->type_int * rT->type_float);
                typeVal.type = FLOAT;
            }
            else {
                typeVal.type_int = (lT->type_int * rT->type_int);
                typeVal.type = INT;
            }
        }
        break;
    case DIV:
        if (lT->type == FLOAT) {
            if (rT->type == FLOAT)
                typeVal.type_float = (lT->type_float / rT->type_float);
            else
                typeVal.type_float = (lT->type_float / rT->type_int);
            typeVal.type = FLOAT;
        } else {
            if (rT->type == FLOAT) {
                typeVal.type_float = (lT->type_int / rT->type_float);
                typeVal.type = FLOAT;
            }
            else {
                typeVal.type_int = (lT->type_int / rT->type_int);
                typeVal.type = INT;
            }
        }
        break;
    default:
        break;
    }
    return typeVal;
}

static int getType(char *text) {
    if (strcmp(text, "int") == 0) {
        return INT;
    } else if (strcmp(text, "float") == 0) {
        return FLOAT;
    } else if (strcmp(text, "char") == 0) {
        return CHAR;
    } else {
        return VOID;
    }
}

static string displayType(int type) {
    switch (type) {
    case INT:
        return "int";
    case FLOAT:
        return "float";
    case CHAR:
        return "char";
    case VOID:
        return "void";
    default:
        return "unknown type";
    }
}

static void displayTable() {
    printf("name\tlev\ttype\tflag\tparam\tlink\n");
    for (auto it=symbolTab.cbegin(); it!=symbolTab.cend(); it++) {
        printf("%s\t%d\t%s\t%c\t%d\t%d\n", (*it)->name, (*it)->lev, displayType((*it)->type).c_str(), (*it)->flag, (*it)->param, (*it)->link);
    }
    printf("\n");
    for (auto it=bTab.cbegin(); it!=bTab.cend(); it++) {
        printf("%d\n", *it);
    }
    printf("\n");
}

static int checkUndeclaredVar(const char *name, int pos) {
    int lev, index;
    for (lev = curLev; lev >= 0; --lev) {
        index = bTab[lev];
        while (index != -1) {
            if (strcmp(symbolTab[index]->name, name) == 0) {
                if (symbolTab[index]->flag == 'F') {
                    fprintf(stderr, "Semantic Error: illegal usage of function \"%s\" at line %d\n", name, pos);
                    hasError = 1;
                    return -1;
                }
                return index;
            }
            index = symbolTab[index]->link;
        } 
    }
    fprintf(stderr, "Semantic Error: variable \"%s\" undeclared at line %d\n", name, pos);
    hasError = 1;
    return -1;
}

static int checkUndeclaredFunc(const char *name, int pos, int argc) {
    if (funcTab.count(name) == 0) {
        fprintf(stderr, "Semantic Error: function \"%s\" undeclared at line %d\n", name, pos);
        return -1;
    } else {
        int index = funcTab[name];
        if (symbolTab[index]->param > argc) {
            fprintf(stderr, "Semantic Error: too few arguments to function \"%s\" at line %d\n", name, pos);
            hasError = 1;
            return -1;
        } else if (symbolTab[index]->param > argc) {
            fprintf(stderr, "Semantic Error: too many arguments to function \"%s\" at line %d\n", name, pos);
            hasError = 1;
            return -1;
        }
        return index;
    }
}

static int checkRedeclaration(const char *name, int pos) {
    int index = bTab[curLev];
    while (index != -1) {
        if (strcmp(symbolTab[index]->name, name) == 0) {
            fprintf(stderr, "Semantic Error: redeclaration of \"%s\" at line %d\n", name, pos);
            hasError = 1;
            return -1;
        }
        index = symbolTab[index]->link;
    }
    return 0;
}

static void analysisExtVarDef(ASTNode *T) {
    if (bTab.empty()) {
        bTab.push_back(-1);
    }
    isExtVar = 1;
    curType = getType(T->ptr[0]->type_id);
    if (curType == VOID) {
        fprintf(stderr, "Semantic Error: variable declared void at line %d\n", T->pos);
        hasError = 1;
    } else {
        analysis(T->ptr[1]);    //EXT_DEC_LIST
    }
    isExtVar = 0;
}

static void analysisVarDec(ASTNode *T) {
    Symbol *symbol;
    if (checkRedeclaration(T->type_id, T->pos) != -1) {
        symbol = new Symbol();
        strcpy(symbol->name, T->type_id);
        symbol->flag = 'V';
        symbol->type = curType;
        symbol->lev = curLev;
        newAlias(symbol->alias);
        symbol->link = bTab[curLev];
        symbolTab.push_back(symbol);
        bTab[curLev] = symbolTab.size() - 1;
        T->place = bTab[curLev];
    }
    //#ifdef DEBUG
    displayTable();
    //#endif
    if (T->ptr[0]) {
        analysis(T->ptr[0]);
        if (isExtVar && (T->ptr[0]->type != T->ptr[0]->kind)) {
            fprintf(stderr, "Semantic Error: initializer element is not constant at line %d\n", T->pos);
            hasError = 1;
        } else if (T->ptr[0]->type == VOID) {
            fprintf(stderr, "Semantic Error: void value not ignored as it ought to be at line %d\n", T->pos);
            hasError = 1;
        }
    }
}

static void analysisFuncDec(ASTNode *T) {
    Symbol *symbol;
    ASTNode *T0;
    int cnt = 0;
    if (checkRedeclaration(T->type_id, T->pos) != -1) {
        symbol = new Symbol();
        strcpy(symbol->name, T->type_id);
        symbol->flag = 'F';
        symbol->type = funcType;
        symbol->lev = 0;
        T0 = T->ptr[0];
        while (T0) {
            cnt++;
            T0 = T0->ptr[1];
        }
        symbol->param = cnt;
        symbol->link = bTab[curLev];
        symbolTab.push_back(symbol);
        bTab[curLev] = symbolTab.size() - 1;
        T->place = funcTab[T->type_id] = bTab[curLev];
    }
    //#ifdef DEBUG
    displayTable();
    //#endif
    curLev++;
    if (bTab.size() == curLev) {
        bTab.push_back(-1);
    } else {
        bTab[curLev] = -1;
    }
    analysis(T->ptr[0]);        //PARAM_LIST
}

static void analysisParamList(ASTNode *T) {
    analysis(T->ptr[0]);        //PARAM_DEC
    analysis(T->ptr[1]);        //PARAM_LIST
}

static void analysisParamDec(ASTNode *T) {
    Symbol *symbol;
    if (checkRedeclaration(T->type_id, T->pos) != -1) {
        symbol = new Symbol();
        curType = getType(T->ptr[0]->type_id);
        strcpy(symbol->name, T->type_id);
        symbol->flag = 'P';
        symbol->type = curType;
        symbol->lev = 1;
        newAlias(symbol->alias);
        symbol->link = bTab[curLev];
        symbolTab.push_back(symbol);
        bTab[curLev] = symbolTab.size() - 1;
        T->place = bTab[curLev];
    }
    //#ifdef DEBUG
    displayTable();
    //#endif
}

static void analysisCompStm(ASTNode *T) {
    if (!isFuncDef) {
        curLev++;
        if (bTab.size() == curLev) {
            bTab.push_back(-1);
        } else {
            bTab[curLev] = -1;
        }
    }
    isFuncDef = 0;
    analysis(T->ptr[0]);        //STM_LIST
    //#ifdef DEBUG
    displayTable();
    //#endif
    curLev--;
}

static void analysisVarDef(ASTNode *T) {
    curType = getType(T->ptr[0]->type_id);
    if (curType == VOID) {
        fprintf(stderr, "Semantic Error: variable declared void at line %d\n", T->pos);
        hasError = 1;
    } else {
        analysis(T->ptr[1]);    //VAR_DEC_LIST
    }
}

static void analysisReturn(ASTNode *T) {
    if (T->ptr[0]) {
        analysis(T->ptr[0]);    //EXP_STMT
        if (funcType == VOID && T->ptr[0]->type != VOID) {
            fprintf(stderr, "Semantic Error: return with a value, in function returning void at line %d\n", T->pos);
            hasError = 1;
        } else if (funcType != VOID && T->ptr[0]->type == VOID) {
            fprintf(stderr, "Semantic Error: return with no value, in function returning non-void at line %d\n", T->pos);
            hasError = 1;
        }
    } else {
        if (funcType != VOID) {
            fprintf(stderr, "Semantic Error: return with no value, in function returning non-void at line %d\n", T->pos);
            hasError = 1;
        }
    }
}

static void analysisAssignop(ASTNode *T) {
    analysis(T->ptr[0]);
    analysis(T->ptr[1]);
    if (T->ptr[0]->kind != ID) {
        fprintf(stderr, "Semantic Error: lvalue required as left operand of assignment at line %d\n", T->pos);
        hasError = 1;
    } else if (T->ptr[1]->type == VOID) {
        fprintf(stderr, "Semantic Error: void value not ignored as it ought to be at line %d\n", T->pos);
        hasError = 1;
    }
    T->type = T->ptr[0]->type;
}

static void analysisCompAssign(ASTNode *T) {
    analysis(T->ptr[0]);
    analysis(T->ptr[1]);
    if (T->ptr[0]->kind != ID) {
        fprintf(stderr, "Semantic Error: lvalue required as left operand of assignment at line %d\n", T->pos);
        hasError = 1;
    }
    if (T->type_id[0]=='<' || T->type_id[0]=='>' || T->type_id[0]=='^' || T->type_id[0]=='&' || T->type_id[0]=='|') {
        if (T->ptr[0]->type == FLOAT || T->ptr[1]->type == FLOAT) {
            fprintf(stderr, "Semantic Error: invalid operands to binary %s at line %d (have \"%s\" and \"%s\")\n", 
            T->type_id, T->pos, displayType(T->ptr[0]->type).c_str(), displayType(T->ptr[1]->type).c_str());
            hasError = 1;
        }
    } else if (T->ptr[1]->type == VOID) {
        fprintf(stderr, "Semantic Error: void value not ignored as it ought to be at line %d\n", T->pos);
        hasError = 1;
    }
    T->type = T->ptr[0]->type;
}

static void analysisAnd(ASTNode *T) {
    analysis(T->ptr[0]);
    analysis(T->ptr[1]);
    if (T->ptr[0]->kind == T->ptr[0]->type && T->ptr[0]->type_int == 0) {
        T->kind = INT;
        T->type_int = 0;
        free(T->ptr[0]);
        free(T->ptr[1]);
    } else if (T->ptr[0]->kind == T->ptr[0]->type && T->ptr[1]->kind == T->ptr[1]->type) {
        T->kind = INT;
        T->type_int = T->ptr[0]->type_int && T->ptr[1]->type_int;
        free(T->ptr[0]);
        free(T->ptr[1]);
    } else if (T->ptr[0]->type == VOID || T->ptr[1]->type == VOID) {
        fprintf(stderr, "Semantic Error: void value not ignored as it ought to be at line %d\n", T->pos);
        hasError = 1;
    }
    T->type = INT;
}

static void analysisOr(ASTNode *T) {
    analysis(T->ptr[0]);
    analysis(T->ptr[1]);
    if (T->ptr[0]->kind == T->ptr[0]->type && T->ptr[0]->type_int == 1) {
        T->kind = INT;
        T->type_int = 1;
        free(T->ptr[0]);
        free(T->ptr[1]);
    } else if (T->ptr[0]->kind == T->ptr[0]->type && T->ptr[1]->kind == T->ptr[1]->type) {
        T->kind = INT;
        T->type_int = T->ptr[0]->type_int || T->ptr[1]->type_int;
        free(T->ptr[0]);
        free(T->ptr[1]);
    } else if (T->ptr[0]->type == VOID || T->ptr[1]->type == VOID) {
        fprintf(stderr, "Semantic Error: void value not ignored as it ought to be at line %d\n", T->pos);
        hasError = 1;
    }
    T->type = INT;
}

static void analysisRelop(ASTNode *T) {
    analysis(T->ptr[0]);
    analysis(T->ptr[1]);
    if (T->ptr[0]->kind == T->ptr[0]->type && T->ptr[1]->kind == T->ptr[1]->type) {
        T->type_int = getTypeVal(T->ptr[0], T->ptr[1], RELOP, T->type_id).type_int;
        T->kind = INT;
        free(T->ptr[0]);
        free(T->ptr[1]);
    } else if (T->ptr[0]->type == VOID || T->ptr[1]->type == VOID) {
        fprintf(stderr, "Semantic Error: void value not ignored as it ought to be at line %d\n", T->pos);
        hasError = 1;
    }
    T->type = INT;
}

static void analysisArithOp(ASTNode *T) {
    analysis(T->ptr[0]);
    analysis(T->ptr[1]);
    if (T->ptr[0]->kind == T->ptr[0]->type && T->ptr[1]->kind == T->ptr[1]->type) {
        TypeVal typeVal = getTypeVal(T->ptr[0], T->ptr[1], T->kind, NULL);
        T->type = T->kind = typeVal.type;
        if (T->kind == FLOAT) 
            T->type_float = typeVal.type_float;
        else 
            T->type_int = typeVal.type_int;
        free(T->ptr[0]);
        free(T->ptr[1]);
    } else if (T->ptr[0]->type == VOID || T->ptr[1]->type == VOID) {
        fprintf(stderr, "Semantic Error: void value not ignored as it ought to be at line %d\n", T->pos);
        hasError = 1;
    } else {
        if (T->ptr[0]->type == FLOAT || T->ptr[1]->type == FLOAT)
            T->type = FLOAT;
        else
            T->type = INT;
    }
}

static void analysisBinaryOp(ASTNode *T) {
    char op[3];
    switch (T->kind) {
    case MOD:
        strcpy(op, "\%");
        break;
    case BITAND:
        strcpy(op, "&");
        break;
    case BITOR:
        strcpy(op, "|");
        break;
    case BITXOR:
        strcpy(op, "^");
        break;
    case BITSHL:
        strcpy(op, "<<");
        break;
    case BITSHR:
        strcpy(op, ">>");
        break;
    }
    analysis(T->ptr[0]);
    analysis(T->ptr[1]);
    if (T->ptr[0]->type == FLOAT || T->ptr[1]->type == FLOAT) {
        fprintf(stderr, "Semantic Error: invalid operands to binary %s at line %d (have \"%s\" and \"%s\")\n", 
                op, T->pos, displayType(T->ptr[0]->type).c_str(), displayType(T->ptr[1]->type).c_str());
        hasError = 1;
    } else if (T->ptr[0]->type == VOID || T->ptr[1]->type == VOID) {
        fprintf(stderr, "Semantic Error: void value not ignored as it ought to be at line %d\n", T->pos);
        hasError = 1;
    } else {
        if (T->ptr[0]->kind == INT && T->ptr[1]->kind == INT) {
            T->type_int = T->ptr[0]->type_int % T->ptr[1]->type_int;
            T->kind = INT;
            free(T->ptr[0]);
            free(T->ptr[1]);
        }
    }
    T->type = INT;
}

static void analysisNot(ASTNode *T) {
    analysis(T->ptr[0]);
    if (T->ptr[0]->kind == T->ptr[0]->type) {
        T->type_int = !T->ptr[0]->type_int;
        T->kind = INT;
    } else if (T->ptr[0]->type == VOID) {
        fprintf(stderr, "Semantic Error: void value not ignored as it ought to be at line %d\n", T->pos);
        hasError = 1;
    }
    T->type = INT;
}

static void analysisUminus(ASTNode *T) {
    analysis(T->ptr[0]);
    if (T->ptr[0]->kind == INT) {
        T->type_int = -T->ptr[0]->type_int;
        T->kind = INT;
    } else if (T->ptr[0]->kind == FLOAT) {
        T->type_float = -T->ptr[0]->type_float;
        T->kind = FLOAT;
    } else if (T->ptr[0]->type == VOID) {
        fprintf(stderr, "Semantic Error: void value not ignored as it ought to be at line %d\n", T->pos);
        hasError = 1;
    }
    T->type = T->ptr[0]->type;
}

static void analysisFuncCall(ASTNode *T) {
    Symbol *symbol;
    ASTNode *T0;
    int cnt = 0;
    T0 = T->ptr[0];
    while (T0) {
        cnt++;
        T0 = T0->ptr[1];
    }
    int place = checkUndeclaredFunc(T->type_id, T->pos, cnt);
    if (place != -1) {
        T->type = symbolTab[place]->type;
    }
    analysis(T->ptr[0]);        //ARGS
}

void analysis(ASTNode *T) {
    if (T) {
        switch (T->kind) {
        case EXT_DEF_LIST:
            analysis(T->ptr[0]);        //EXT_VAR_DEF|FUNC_DEF
            analysis(T->ptr[1]);        //EXT_DEF_LIST
            break;
        case EXT_VAR_DEF:
            analysisExtVarDef(T);
            break;
        case EXT_DEC_LIST:
            analysis(T->ptr[0]);        //VAR_DEC
            analysis(T->ptr[1]);        //EXT_DEC_LIST
            break;
        case VAR_DEC:
            analysisVarDec(T);
            break;
        case FUNC_DEF:
            funcType = getType(T->ptr[0]->type_id);
            analysis(T->ptr[1]);        //FUNC_DEC
            isFuncDef = 1;
            analysis(T->ptr[2]);        //COMP_STM
            break;
        case FUNC_DEC:
            analysisFuncDec(T);
            break;
        case PARAM_LIST:
            analysisParamList(T);
            break;
        case PARAM_DEC:
            analysisParamDec(T);
            break;
        case COMP_STM:
            analysisCompStm(T);
            break;
        case STM_LIST:
            analysis(T->ptr[0]);        //EXP_STMT, RETURN...
            analysis(T->ptr[1]);        //STM_LIST
            break;
        case VAR_DEF:
            analysisVarDef(T);
            break;
        case VAR_DEC_LIST:
            analysis(T->ptr[0]);        //VAR_DEC
            analysis(T->ptr[1]);        //VAR_DEC_LIST
            break;
        case EXP_STMT:
            analysis(T->ptr[0]);
            break;
        case RETURN:
            analysisReturn(T);
            break;
        case IF_THEN:
            analysis(T->ptr[0]);        //EXP
            analysis(T->ptr[1]);        //STMT
            break;
        case IF_THEN_ELSE:
            analysis(T->ptr[0]);        //EXP
            analysis(T->ptr[1]);        //STMT
            analysis(T->ptr[1]);        //STMT
            break;
        case WHILE:
            analysis(T->ptr[0]);        //EXP
            inLoop = 1;
            analysis(T->ptr[1]);        //STMT
            inLoop = 0;
            break;
        case CONTINUE:
            if (!inLoop) {
                fprintf(stderr, "Semantic Error: continue statement not within a loop at line %d\n", T->pos);
                hasError = 1;
            }
            break;
        case BREAK:
            if (!inLoop) {
                fprintf(stderr, "Semantic Error: break statement not within a loop at line %d\n", T->pos);
                hasError = 1;
            }
            break;
        case ID:
            T->place = checkUndeclaredVar(T->type_id, T->pos);
            if (T->place != -1) {
                T->type = symbolTab[T->place]->type;
            }
            break;
        case INT:
            T->type = INT;
            break;
        case FLOAT:
            T->type = FLOAT;
            break;
        case ASSIGNOP:
            analysisAssignop(T);
            break;
        case COMP_ASSIGN:
            analysisCompAssign(T);
            break;
        case AND:
            analysisAnd(T);
            break;
        case OR:
            analysisOr(T);
            break;
        case RELOP:
            analysisRelop(T);
            break;
        case PLUS:
        case MINUS:
        case STAR:
        case DIV:
            analysisArithOp(T);
            break;
        case MOD:
        case BITAND:
        case BITOR:
        case BITXOR:
        case BITSHL:
        case BITSHR:
            analysisBinaryOp(T);
            break;
        case NOT:
            analysisNot(T);
            break;
        case UMINUS:
            analysisUminus(T);
            break;
        case DPLUS:
            if (T->ptr[0]->kind != ID) {
                fprintf(stderr, "Semantic Error: lvalue required as increment operand at line %d\n", T->pos);
                hasError = 1;
            }
            break;
        case DMINUS:
            if (T->ptr[0]->kind != ID) {
                fprintf(stderr, "Semantic Error: lvalue required as decrement operand at line %d\n", T->pos);
                hasError = 1;
            }
            break;
        case FUNC_CALL:
            analysisFuncCall(T);
            break;
        case ARGS:
            analysis(T->ptr[0]);        //EXP
            analysis(T->ptr[1]);        //ARGS
            break;
        }
        //To Do
        //displayTAC(T->code);
    }
}

static void displayTAC(TACNode *head) {
    TACNode *node = head;
    do {
        switch(node->op) {
        }
        node = node->next;
    } while (node != head);
}

static TACNode *merge(int num, ...) {
    va_list pArgs;
    va_start(pArgs, num);
    TACNode *head;
    int i = 0;
    while ((head = va_arg(pArgs, TACNode *)) == NULL) i++;
    TACNode *head1 = head, *head2, *tail1, *tail2;
    for (; i<num; ++i) {
        head2 = va_arg(pArgs, TACNode *);
        if (head2 == NULL)
            continue;
        tail1 = head1->prior;
        tail2 = head2->prior;
        tail1->next = head2;
        tail2->next = head1;
        head1->prior = tail2;
        head2->prior = tail1;
        head1 = head2;
    }
    return head;
}

static void newLabel(char *label) {
    static int cnt = 0;
    sprintf(label, "L%d", cnt);
    cnt++;
}

static int newTemp(int type) {
    static int cnt = 0;
    Symbol *symbol = new Symbol();
    sprintf(symbol->alias, "T%d", cnt);
    symbol->type = type;
    symbol->flag = 'T';
    symbolTab.push_back(symbol);
    cnt++;
    return symbolTab.size() - 1;
}

static TACNode *gen(int op, Opn opn1, Opn opn2, Opn result) {
    TACNode *node = new TACNode();
    node->op = op;
    node->opn1 = opn1;
    node->opn2 = opn2;
    node->result = result;
    node->next = node->prior = node;
    return node;
}

static TACNode *genLabel(char *label) {
    TACNode *node = new TACNode();
    node->op = LABEL;
    node->result.kind = LABEL;
    strcpy(node->result.label, label);
    return node;
}

static TACNode *genGoto(char *label) {
    TACNode *node = new TACNode();
    node->op = GOTO;
    node->result.kind = LABEL;
    strcpy(node->result.label, label);
    return node;
}

static void genVarDec(ASTNode *T) {
    Opn opn1, opn2, result;
    if (T->ptr[0]) {            //ID ASSIGNOP EXP
        result.kind = VAR;
        result.place = T->place;
        if (T->ptr[0]->kind == T->ptr[0]->type) {
            opn1.kind = T->ptr[0]->type;
            if (T->ptr[0]->type == FLOAT) {
                opn1.const_float = T->ptr[0]->type_float;
            } else {
                opn1.const_int = T->ptr[0]->type_int;
            }
        } else {
            genTAC(T->ptr[0]);  //EXP
            opn1.kind = VAR;
            opn1.place = T->ptr[0]->place;
        }
        T->code = merge(2, T->ptr[0]->code, gen(ASSIGNOP, opn1, opn2, result));
    }
}

static void genFuncDef(ASTNode *T) {
    Opn opn1, opn2, result;
    genTAC(T->ptr[1]);          //FUNC_DEC
    genTAC(T->ptr[2]);          //COMP_STM
    T->code = merge(2, T->ptr[1]->code, T->ptr[2]->code);
    if (T->code->prior->op != RETURN) {
        T->code = merge(2, T->code, gen(RETURN, opn1, opn2, result));
    }
}

static void genFuncDec(ASTNode *T) {
    Opn opn1, opn2, result;
    result.kind = FUNC;
    result.place = T->place;
    if (T->ptr[0]) {
        genTAC(T->ptr[0]);      //PARAM_LIST
        T->code = merge(2, gen(FUNC, opn1, opn2, result), T->ptr[0]->code);
    } else {
        T->code = gen(FUNC, opn1, opn2, result);
    }
}

static void genParamDec(ASTNode *T) {
    Opn opn1, opn2, result;
    result.kind = PARAM;
    result.place = T->place;
    T->code = gen(PARAM, opn1, opn2, result);
}

static void genStmList(ASTNode *T) {
    if (T->ptr[0]) {
        if (T->ptr[1]) {
            genTAC(T->ptr[0]);  //EXP_STMT, RETURN ...
            genTAC(T->ptr[1]);  //STM_LIST
            T->code = merge(2, T->ptr[0]->code, T->ptr[1]->code);
        } else {
            genTAC(T->ptr[0]);  //EXP_STMT, RETURN ...
            T->code = T->ptr[0]->code;
        }
    } else {
        if (T->ptr[1]) {
            genTAC(T->ptr[1]);  //STM_LIST
            T->code = T->ptr[1]->code;
        } else {
            T->code = NULL;
        }
    }
}

static void genReturn(ASTNode *T) {
    Opn opn1, opn2, result;
    if (T->ptr[0]) {
        genTAC(T->ptr[0]);      //EXP
        if (T->ptr[0]->kind == T->ptr[0]->type) {
            result.kind = T->ptr[0]->type;
            if (T->ptr[0]->type == FLOAT) {
                result.const_float = T->ptr[0]->type_float;
            } else {
                result.const_int = T->ptr[0]->type_int;
            }
        } else {
            result.kind = VAR;
            result.place = T->ptr[0]->place;
        }
        T->code = merge(2, T->ptr[0]->code, gen(RETURN, opn1, opn2, result));
    } else {
        T->code = gen(RETURN, opn1, opn2, result);
    }
}

static void genIfThen(ASTNode *T) {
    newLabel(T->Snext);
    newLabel(T->ptr[0]->Etrue);
    strcpy(T->ptr[0]->Efalse, T->Snext);
    genTAC(T->ptr[0]);          //EXP
    genTAC(T->ptr[1]);          //STMT
    T->code = merge(4, T->ptr[0]->code, 
                    genLabel(T->ptr[0]->Etrue), 
                    T->ptr[1]->code, 
                    genLabel(T->Snext));
}

static void genIfThenElse(ASTNode *T) {
    newLabel(T->Snext);
    newLabel(T->ptr[0]->Etrue);
    newLabel(T->ptr[0]->Efalse);
    genTAC(T->ptr[0]);          //EXP
    genTAC(T->ptr[1]);          //STMT
    genTAC(T->ptr[2]);          //STMT
    T->code = merge(7, T->ptr[0]->code, 
                    genLabel(T->ptr[0]->Etrue),
                    T->ptr[1]->code,
                    genGoto(T->Snext),
                    genLabel(T->ptr[0]->Efalse),
                    T->ptr[2]->code,
                    genLabel(T->Snext));
}

static void genWhile(ASTNode *T) {
    newLabel(T->Snext);
    newLabel(T->ptr[0]->Etrue);
    strcpy(T->ptr[0]->Efalse, T->Snext);
    newLabel(T->ptr[1]->Snext);
    genTAC(T->ptr[0]);          //EXP
    genTAC(T->ptr[1]);          //STMT
    T->code = merge(6, genLabel(T->ptr[1]->Snext),
                    T->ptr[0]->code,
                    genLabel(T->ptr[0]->Etrue),
                    T->ptr[1]->code,
                    genGoto(T->ptr[1]->Snext),
                    genLabel(T->Snext));
}

static void genId(ASTNode *T) {
    Opn opn1, opn2, result;
    if (T->Etrue[0]) {
        opn1.kind = VAR;
        opn1.place = T->place;
        opn2.kind = INT;        //0
        result.kind = LABEL;
        strcpy(result.label, T->Etrue);
        T->code = merge(2, gen(NEQ, opn1, opn2, result), genGoto(T->Efalse));
    }
}

static void genAssignop(ASTNode *T) {
    Opn opn1, opn2, result;
    T->place = T->ptr[0]->place;
    result.kind = VAR;
    result.place = T->place;    //T->place == T->ptr[0]->place
    if (T->ptr[1]->kind == T->ptr[1]->type) {
        opn1.kind = T->ptr[1]->type;
        if (T->ptr[1]->type == FLOAT) {
            opn1.const_float = T->ptr[1]->type_float;
        } else {
            opn1.const_int = T->ptr[1]->type_int;
        }
    } else {
        genTAC(T->ptr[1]);      //EXP
        opn1.kind = VAR;
        opn1.place = T->ptr[1]->place;
    }
    T->code = merge(2, T->ptr[1]->code, gen(ASSIGNOP, opn1, opn2, result));
    if (T->Etrue[0]) {
        opn1.kind = VAR;
        opn1.place = T->place;
        opn2.kind = INT;        //0
        result.kind = LABEL;
        strcpy(result.label, T->Etrue);
        T->code = merge(3, T->code, gen(NEQ, opn1, opn2, result), genGoto(T->Efalse));
    }
}

static void genCompAssign(ASTNode *T) {
    Opn opn1, opn2, result;
    T->place = T->ptr[0]->place;
    result.kind = VAR;
    result.place = T->ptr[0]->place;    //T->place == T->ptr[0]->place
    if (T->ptr[1]->kind == T->ptr[1]->type) {
        opn2.kind = T->ptr[1]->type;
        if (T->ptr[1]->type == FLOAT) {
            opn2.const_float = T->ptr[1]->type_float;
        } else {
            opn2.const_int = T->ptr[1]->type_int;
        }
    } else {
        genTAC(T->ptr[1]);      //EXP
        opn2.kind = VAR;
        opn2.place = T->ptr[1]->place;
    }
    switch (T->type_id[0]) {
    case '+':
        T->code = merge(2, T->ptr[1]->code, gen(PLUS, result, opn2, result));
        break;
    case '-':
        T->code = merge(2, T->ptr[1]->code, gen(MINUS, result, opn2, result));
        break;
    case '*':
        T->code = merge(2, T->ptr[1]->code, gen(STAR, result, opn2, result));
        break;
    case '/':
        T->code = merge(2, T->ptr[1]->code, gen(DIV, result, opn2, result));
        break;
    case '%':
        T->code = merge(2, T->ptr[1]->code, gen(MOD, result, opn2, result));
        break;
    case '&':
        T->code = merge(2, T->ptr[1]->code, gen(BITAND, result, opn2, result));
        break;
    case '|':
        T->code = merge(2, T->ptr[1]->code, gen(BITOR, result, opn2, result));
        break;
    case '^':
        T->code = merge(2, T->ptr[1]->code, gen(BITXOR, result, opn2, result));
        break;
    case '<':
        T->code = merge(2, T->ptr[1]->code, gen(BITSHL, result, opn2, result));
        break;
    case '>':
        T->code = merge(2, T->ptr[1]->code, gen(BITSHR, result, opn2, result));
        break;
    }
    if (T->Etrue[0]) {
        opn1.kind = VAR;
        opn1.place = T->place;
        opn2.kind = INT;        //0
        opn2.const_int = 0;
        result.kind = LABEL;
        strcpy(result.label, T->Etrue);
        T->code = merge(3, T->code, gen(NEQ, opn1, opn2, result), genGoto(T->Efalse));
    }
}

static void genAnd(ASTNode *T) {
    if (T->Etrue[0]) {
        newLabel(T->ptr[0]->Etrue);
        strcpy(T->ptr[1]->Etrue, T->Etrue);
        strcpy(T->ptr[0]->Efalse, T->Efalse);
        strcpy(T->ptr[1]->Efalse, T->Efalse);
        genTAC(T->ptr[0]);
        genTAC(T->ptr[1]);
        T->code = merge(3, T->ptr[0]->code, genLabel(T->ptr[0]->Etrue), T->ptr[1]->code);
    } else {
        Opn opn1, opn2, result;
        T->place = newTemp(T->type);
        if (T->ptr[0]->kind == T->ptr[0]->type) {   //1 && EXP
            char label1[8];
            TACNode *code1, *code2, *code3;
            genTAC(T->ptr[1]);      //EXP
            newLabel(label1);
            opn1.kind = INT;
            opn1.const_int = 0;
            result.kind = VAR;
            result.place = T->place;
            code1 = gen(ASSIGNOP, opn1, opn2, result);  //t1 := 0
            opn1.const_int = 1;
            code3 = gen(ASSIGNOP, opn1, opn2, result);  //t1 := 1
            opn1.kind = VAR;
            opn1.place = T->ptr[1]->place;
            opn2.kind = INT;        //0
            result.kind = LABEL;
            strcpy(result.label, label1);
            code2 = gen(EQ, opn1, opn2, result);        //if T2 == 0 goto label1
            T->code = merge(5, code1, T->ptr[1]->code, code2, code3, genLabel(label1));
        } else {                                    //EXP && EXP
            char label1[8];
            TACNode *code1, *code2, *code3, *code4;
            genTAC(T->ptr[0]);  //EXP
            newLabel(label1);
            opn1.kind = INT;
            opn1.const_int = 0;
            result.kind = VAR;
            result.place = T->place;
            code1 = gen(ASSIGNOP, opn1, opn2, result);  //t1 := 0
            opn1.const_int = 1;
            code4 = gen(ASSIGNOP, opn1, opn2, result);  //t1 := 1
            opn1.kind = VAR;
            opn1.place = T->ptr[0]->place;
            opn2.kind = INT;        //0
            result.kind = LABEL;
            strcpy(result.label, label1);
            code2 = gen(EQ, opn1, opn2, result);        //if T1 == 0 goto label1
            if (T->ptr[1]->kind == T->ptr[1]->type) {
                if (T->ptr[1]->type_int) {
                    T->code = merge(5, code1, T->ptr[0]->code, code2, code4, genLabel(label1));
                } else {
                    T->code = merge(2, code1, T->ptr[0]->code);
                }
            } else {
                genTAC(T->ptr[1]);
                opn1.place = T->ptr[1]->place;
                code3 = gen(EQ, opn1, opn2, result);    //if T2 == 0 goto label1
                T->code = merge(7, code1, T->ptr[0]->code, code2, T->ptr[1]->code, code3, code4, genLabel(label1));
            }
        }
    }
}

static void genOr(ASTNode *T) {
    if (T->Etrue[0]) {
                newLabel(T->ptr[0]->Efalse);
                strcpy(T->ptr[0]->Etrue, T->Etrue);
                strcpy(T->ptr[1]->Etrue, T->Etrue);
                strcpy(T->ptr[1]->Efalse, T->Efalse);
                genTAC(T->ptr[0]);
                genTAC(T->ptr[1]);
                T->code = merge(3, T->ptr[0]->code, genLabel(T->ptr[0]->Efalse), T->ptr[1]->code);
    } else {
        Opn opn1, opn2, result;
        T->place = newTemp(T->type);
        if (T->ptr[0]->kind == T->ptr[0]->type) {   //1 && EXP
            char label1[8];
            TACNode *code1, *code2, *code3;
            genTAC(T->ptr[1]);      //EXP
            newLabel(label1);
            opn1.kind = INT;
            opn1.const_int = 0;
            result.kind = VAR;
            result.place = T->place;
            code1 = gen(ASSIGNOP, opn1, opn2, result);  //t1 := 0
            opn1.const_int = 1;
            code3 = gen(ASSIGNOP, opn1, opn2, result);  //t1 := 1
            opn1.kind = VAR;
            opn1.place = T->ptr[1]->place;
            opn2.kind = INT;        //0
            result.kind = LABEL;
            strcpy(result.label, label1);
            code2 = gen(EQ, opn1, opn2, result);        //if T2 == 0 goto label1
            T->code = merge(5, code1, T->ptr[1]->code, code2, code3, genLabel(label1));
        } else {                                    //EXP && EXP
            char label1[8];
            TACNode *code1, *code2, *code3, *code4;
            genTAC(T->ptr[0]);  //EXP
            newLabel(label1);
            opn1.kind = INT;
            opn1.const_int = 1;
            result.kind = VAR;
            result.place = T->place;
            code1 = gen(ASSIGNOP, opn1, opn2, result);  //t1 := 1
            opn1.const_int = 0;
            code4 = gen(ASSIGNOP, opn1, opn2, result);  //t1 := 0
            opn1.kind = VAR;
            opn1.place = T->ptr[0]->place;
            opn2.kind = INT;        //0
            result.kind = LABEL;
            strcpy(result.label, label1);
            code2 = gen(NEQ, opn1, opn2, result);       //if T1 != 0 goto label1
            if (T->ptr[1]->kind == T->ptr[1]->type) {
                if (T->ptr[1]->type_int) {
                    T->code = merge(2, code1, T->ptr[0]->code);
                } else {
                    T->code = merge(5, code1, T->ptr[0]->code, code2, code4, genLabel(label1));
                }
            } else {
                genTAC(T->ptr[1]);
                opn1.place = T->ptr[1]->place;
                code3 = gen(NEQ, opn1, opn2, result);   //if T2 != 0 goto label1
                T->code = merge(7, code1, T->ptr[0]->code, code2, T->ptr[1]->code, code3, code4, genLabel(label1));
            }
        }
    }
}

static void genNot(ASTNode *T) {
    if (T->Etrue[0]) {
        strcpy(T->ptr[0]->Etrue, T->Efalse);
        strcpy(T->ptr[0]->Efalse, T->Etrue);
        genTAC(T->ptr[0]);
        T->code = T->ptr[0]->code;
    } else {
        Opn opn1, opn2, result;
        char label1[8];
        TACNode *code1, *code2, *code3;
        T->place = newTemp(T->type);
        genTAC(T->ptr[0]);
        newLabel(label1);
        opn1.kind = INT;
        opn1.const_int = 1;
        result.kind = VAR;
        result.place = T->place;
        code1 = gen(ASSIGNOP, opn1, opn2, result);  //t1 := 1
        opn1.const_int = 0;
        code3 = gen(ASSIGNOP, opn1, opn2, result);  //t1 := 0
        opn1.kind = VAR;
        opn1.place = T->ptr[0]->place;
        opn2.kind = INT;        //0
        result.kind = LABEL;
        strcpy(result.label, label1);
        code2 = gen(EQ, opn1, opn2, result);       //if T1 == 0 goto label1
        T->code = merge(5, T->ptr[0]->code, code1, code2, code3, genLabel(label1));
    }
}

static void genUminus(ASTNode *T) {
    Opn opn1, opn2, result;
    T->place = newTemp(T->type);
    genTAC(T->ptr[0]);
    opn1.kind = VAR;
    opn1.place = T->ptr[0]->place;
    result.kind = VAR;
    result.place = T->place;
    T->code = merge(2, T->ptr[0]->code, gen(UMINUS, opn1, opn2, result));
    if (T->Etrue[0]) {
        opn1.kind = VAR;
        opn1.place = T->place;
        opn2.kind = INT;        //0
        result.kind = LABEL;
        strcpy(result.label, T->Etrue);
        T->code = merge(3, T->code, gen(NEQ, opn1, opn2, result), genGoto(T->Efalse));
    }
}

static void genRelop(ASTNode *T) {
    int kind;
    Opn opn1, opn2, result;
    genTAC(T->ptr[0]);
    genTAC(T->ptr[1]);
    if (T->ptr[0]->kind == T->ptr[0]->type) {
        if (T->ptr[0]->kind == FLOAT) {
            opn1.kind = FLOAT;
            opn1.const_float = T->ptr[0]->type_float;
        } else {
            opn1.kind = INT;
            opn1.const_float = T->ptr[0]->type_int;
        }
        opn2.kind = VAR;
        opn2.place = T->ptr[1]->place;
    } else if (T->ptr[1]->kind == T->ptr[1]->type) {
        if (T->ptr[1]->kind == FLOAT) {
            opn2.kind = FLOAT;
            opn2.const_float = T->ptr[1]->type_float;
        } else {
            opn2.kind = INT;
            opn2.const_float = T->ptr[1]->type_int;
        }
        opn1.kind = VAR;
        opn1.place = T->ptr[1]->place;
    } else {
        opn1.kind = VAR;
        opn1.place = T->ptr[0]->place;
        opn2.kind = VAR;
        opn2.place = T->ptr[1]->place;
    }
    switch (T->type_id[0]) {
    case '=':
        kind = EQ;
        break;
    case '!':
        kind = NEQ;
        break;
    case '>':
        if (T->type_id[1]) {
            kind = GE;
        } else {
            kind = GT;
        }
        break;
    case '<':
        if (T->type_id[1]) {
            kind = LE;
        } else {
            kind = LT;
        }
        break;
    }
    if (T->Etrue[0]) {
        result.kind = LABEL;
        strcpy(result.label, T->Etrue);
        T->code = merge(4, T->ptr[0]->code, T->ptr[1]->code, gen(kind, opn1, opn2, result), genGoto(T->Efalse));
    } else {
        char label1[8];
        TACNode *code1, *code2, *code3;
        T->place = newTemp(T->type);
        newLabel(label1);
        result.kind = LABEL;
        strcpy(result.label, label1);
        code2 = gen(kind, opn1, opn2, result);        //if T1 relop T2 goto label1
        opn1.kind = INT;
        opn1.const_int = 1;
        result.kind = VAR;
        result.place = T->place;
        code1 = gen(ASSIGNOP, opn1, opn2, result);  //t1 := 1
        opn1.const_int = 0;
        code3 = gen(ASSIGNOP, opn1, opn2, result);  //t1 := 0
        T->code = merge(6, T->ptr[0]->code, T->ptr[1]->code, code1, code2, code3, genLabel(label1));
    }
}

static void genArithOp(ASTNode *T) {
    Opn opn1, opn2, result;
    T->place = newTemp(T->type);
    genTAC(T->ptr[0]);
    genTAC(T->ptr[1]);
    if (T->ptr[0]->kind == T->ptr[0]->type) {
        if (T->ptr[0]->kind == FLOAT) {
            opn1.kind = FLOAT;
            opn1.const_float = T->ptr[0]->type_float;
        } else {
            opn1.kind = INT;
            opn1.const_float = T->ptr[0]->type_int;
        }
        opn2.kind = VAR;
        opn2.place = T->ptr[1]->place;
    } else if (T->ptr[1]->kind == T->ptr[1]->type) {
        if (T->ptr[1]->kind == FLOAT) {
            opn2.kind = FLOAT;
            opn2.const_float = T->ptr[1]->type_float;
        } else {
            opn2.kind = INT;
            opn2.const_float = T->ptr[1]->type_int;
        }
        opn1.kind = VAR;
        opn1.place = T->ptr[1]->place;
    } else {
        opn1.kind = VAR;
        opn1.place = T->ptr[0]->place;
        opn2.kind = VAR;
        opn2.place = T->ptr[1]->place;
    }
    result.kind = VAR;
    result.place = T->place;
    T->code = merge(3, T->ptr[0]->code, T->ptr[1]->code, gen(T->kind, opn1, opn2, result));
    if (T->Etrue[0]) {
        opn1.kind = VAR;
        opn1.place = T->place;
        opn2.kind = INT;        //0
        opn2.const_int = 0;
        result.kind = LABEL;
        strcpy(result.label, T->Etrue);
        T->code = merge(3, T->code, gen(NEQ, opn1, opn2, result), genGoto(T->Efalse));
    }
}

static void genDPLusMinus(ASTNode *T) {
    Opn opn1, opn2, result;
    TACNode *code1, *code2;
    opn2.kind = INT;
    opn2.const_int = 1;
    result.kind = VAR;
    result.place = T->ptr[0]->place;
    if (T->kind == DPLUS) {
        code2 = gen(PLUS, result, opn2, result);        //T1 := T1 + 1
        if (T->type_id[0] == 'R') {                     //T1++
            T->place = newTemp(T->type);
            result.kind = VAR;
            result.place = T->place;
            opn1.kind = VAR;
            opn1.place = T->ptr[0]->place;
            code1 = gen(ASSIGNOP, opn1, opn2, result);
            T->code = merge(2, code1, code2);
        } else {                                        //++T1
            T->place = T->ptr[0]->place;
            T->code = code2;
        }
    } else {
        code2 = gen(MINUS, result, opn2, result);       //T1 := T1 - 1
        if (T->type_id[0] == 'R') {                     //T1--
            T->place = newTemp(T->type);
            result.kind = VAR;
            result.place = T->place;
            opn1.kind = VAR;
            opn1.place = T->ptr[0]->place;
            code1 = gen(ASSIGNOP, opn1, opn2, result);
            T->code = merge(2, code1, code2);
        } else {                                        //--T1
            T->place = T->ptr[0]->place;
            T->code = code2;
        }    
    }
    if (T->Etrue[0]) {
        opn1.kind = VAR;
        opn1.place = T->place;
        opn2.kind = INT;        //0
        opn2.const_int = 0;
        result.kind = LABEL;
        strcpy(result.label, T->Etrue);
        T->code = merge(3, T->code, gen(NEQ, opn1, opn2, result), genGoto(T->Efalse));
    }
}

static void genFuncCall(ASTNode *T) {
    Opn opn1, opn2, result;
    genTAC(T->ptr[0]);          //ARGS
    opn1.kind = FUNC;
    opn1.place = funcTab[T->type_id];
    if (T->type != VOID) {
        T->place = newTemp(T->type);
        result.kind = VAR;
        result.place = T->place;
        T->code = merge(2, T->ptr[0]->code, gen(FUNC_CALL, opn1, opn2, result));
    } else {
        T->code = merge(2, T->ptr[0]->code, gen(FUNC_CALL, opn1, opn2, result));
    }
    if (T->Etrue[0]) {
        opn1.kind = VAR;
        opn1.place = T->place;
        opn2.kind = INT;        //0
        opn2.const_int = 0;
        result.kind = LABEL;
        strcpy(result.label, T->Etrue);
        T->code = merge(3, T->code, gen(NEQ, opn1, opn2, result), genGoto(T->Efalse));
    }
}

static void genArgs(ASTNode *T) {
    Opn opn1, opn2, result;
    genTAC(T->ptr[0]);          //EXP
    genTAC(T->ptr[1]);          //ARGS
    if (T->ptr[0]->kind == T->ptr[0]->type) {
        result.kind = T->ptr[0]->type;
        if (T->ptr[0]->type == FLOAT) {
            result.const_float = T->ptr[0]->type_float;
        } else {
            result.const_int = T->ptr[0]->type_int;
        }
    } else {
        result.kind = VAR;
        result.place = T->ptr[0]->place;
    }
    T->code = merge(3, T->ptr[0]->code, gen(ARG, opn1, opn2, result), T->ptr[1]->code);
}

void genTAC(ASTNode *T) {
    if (T) {
        switch (T->kind) {
        case EXT_DEF_LIST:
            genTAC(T->ptr[0]);          //EXT_VAR_DEF|FUNC_DEF
            if (T->ptr[1]) {
                genTAC(T->ptr[1]);      //EXT_DEF_LIST
                T->code = merge(2, T->ptr[0]->code, T->ptr[1]->code);
            } else {
                T->code = T->ptr[0]->code;
            }
            break;
        case EXT_VAR_DEF:
            genTAC(T->ptr[1]);          //EXT_DEC_LIST
            T->code = T->ptr[1]->code;
            break;
        case EXT_DEC_LIST:
            genTAC(T->ptr[0]);          //VAR_DEC
            if (T->ptr[1]) {
                genTAC(T->ptr[1]);      //EXT_DEC_LIST
                T->code = merge(2, T->ptr[0]->code, T->ptr[1]->code);
            } else {
                T->code = T->ptr[0]->code;
            }
            break;
        case VAR_DEC:
            genVarDec(T); 
            break;
        case FUNC_DEF:
            genFuncDef(T);
            break;
        case FUNC_DEC:
            genFuncDec(T);
            break;
        case PARAM_LIST:
            genTAC(T->ptr[0]);          //PARAM_DEC
            if (T->ptr[1]) {
                genTAC(T->ptr[1]);      //PARAM_LIST
                T->code = merge(2, T->ptr[0]->code, T->ptr[1]->code);
            } else {
                T->code = T->ptr[0]->code;
            }
            break;
        case PARAM_DEC:
            genParamDec(T);
            break;
        case COMP_STM:
            genTAC(T->ptr[0]);          //STM_LIST
            T->code = T->ptr[0]->code;
            break;
        case STM_LIST:
            genStmList(T);
            break;
        case VAR_DEF:
            genTAC(T->ptr[1]);          //VAR_DEC_LIST
            T->code = T->ptr[1]->code;
            break;
        case VAR_DEC_LIST:
            genTAC(T->ptr[0]);          //VAR_DEC
            if (T->ptr[1]) {
                genTAC(T->ptr[1]);      //VAR_DEC_LIST
                T->code = merge(2, T->ptr[0]->code, T->ptr[1]->code);
            } else {
                T->code = T->ptr[0]->code;
            }
            break;
        case EXP_STMT:
            genTAC(T->ptr[0]);          //EXP
            T->code = T->ptr[0]->code;
            break;
        case RETURN:
            genReturn(T);
            break;
        case IF_THEN:
            genIfThen(T);
            break;
        case IF_THEN_ELSE:
            genIfThenElse(T);
            break;
        case WHILE:
            genWhile(T);
            break;
        case CONTINUE:
            //TODO
            break;
        case BREAK:
            //TODO
            break;
        case ID:
            genId(T);
            break;
        case INT:
            if (T->Etrue[0]) {
                if (T->type_int) {
                    T->code = genGoto(T->Etrue);
                } else {
                    T->code = genGoto(T->Efalse);
                }
            }
            break;
        case FLOAT:
            if (T->Etrue[0]) {
                if (T->type_float) {
                    T->code = genGoto(T->Etrue);
                } else {
                    T->code = genGoto(T->Efalse);
                }
            }
            break;
        case ASSIGNOP:
            genAssignop(T);
            break;
        case COMP_ASSIGN:
            genCompAssign(T);
            break;
        case AND:
            genAnd(T);
            break;
        case OR:
            genOr(T);
            break;
        case RELOP:
            genRelop(T);
            break;
        case PLUS:
        case MINUS:
        case STAR:
        case DIV:
        case MOD:
        case BITAND:
        case BITOR:
        case BITXOR:
        case BITSHL:
        case BITSHR:
            genArithOp(T);
            break;
        case NOT:
            genNot(T);
            break;
        case UMINUS:
            genUminus(T);
            break;
        case DPLUS:
        case DMINUS:
            genDPLusMinus(T);
            break;
        case FUNC_CALL:
            genFuncCall(T);
            break;
        case ARGS:
            break;
        }
    }
}