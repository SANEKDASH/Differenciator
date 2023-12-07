#ifndef PARSE_HEADER
#define PARSE_HEADER

#include "trees.h"

struct Variable
{
    double value;
};

TreeNode *GetG(const char *str);

TreeNode *GetE();

TreeNode *GetP();

TreeNode *GetT();

TreeNode* GetN();

TreeNode *GetId();

TreeNode *GetA();

TreeNode *GetO();

#endif
