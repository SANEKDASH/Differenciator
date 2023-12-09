#ifndef DIFF_HEADER
#define DIFF_HEADER

#include "trees.h"
#include "parse.h"

typedef enum
{
    kDiffSuccess,
    kNotANumber,
    kSyntaxError,
    kDiffFailedAlloc,
} DiffErrs_t;

struct Operation
{
    OpCode_t op_code;
    const char *op_str;
    const char *tex_str;
};

static const Operation OperationArray[] =
{
    kAdd,   "+",    "+",
    kSub,   "-",    "-",
    kMult,  "*",    "\\cdot",
    kDiv,   "/",    "\\frac",
    kSqrt,  "sqrt", "\\sqrt",
    kSin,   "sin",  "\\sin",
    kCos,   "cos",  "\\cos",
    kTg,    "tg",   "\\tg",
    kLn,    "ln",   "\\ln",
    kExp,   "^",    "^",
};

static const size_t kOperationCount = sizeof(OperationArray) / sizeof(Operation);

OpCode_t SeekOperator(const char *op_str);

DiffErrs_t SetNumber(TreeNode   *node,
                     const char *num_str);

DiffErrs_t SetUpData(TreeNode *node,
                     const char *str);

NumType_t Eval(Variables      *vars,
               const TreeNode *node);

TreeNode *DiffTree(const TreeNode *node,
                   TreeNode       *parent_node);

TreeErrs_t OptimizeConstants(Variables *vars,
                             Tree      *tree,
                             TreeNode **node);

TreeErrs_t OptimizeNeutralExpr(Tree      *tree,
                               TreeNode **node);

TreeErrs_t OptimizeTree(Variables *vars,
                        Tree *tree);

bool IsUnaryOp(const OpCode_t op_code);

#endif
