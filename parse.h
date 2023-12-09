#ifndef PARSE_HEADER
#define PARSE_HEADER

#include "trees.h"

typedef double VarVal_t;

struct Expr
{
    const char *string = nullptr;
    size_t pos = 0;
};

struct Variable
{
    char *id = nullptr;

    VarVal_t value = 0;;

};

struct Variables
{
    Variable *var_array;
    size_t size;
    size_t var_count;
};

int VarArrayDtor(Variables *vars);

int AddVar(Variables *vars, const char *var_name);

int SeekVariable(Variables *vars, const char *var_name);

int VarArrayInit(Variables *vars);

TreeNode *GetG(Variables *vars, Expr *expr);

TreeNode *GetE(Variables *vars, Expr *expr);

TreeNode *GetP(Variables *vars, Expr *expr);

TreeNode *GetT(Variables *vars, Expr *expr);

TreeNode* GetN(Variables *vars, Expr *expr);

TreeNode *GetId(Variables *vars, Expr *expr);

TreeNode *GetA(Variables *vars, Expr *expr);

TreeNode *GetO(Variables *vars, Expr *expr);


#endif
