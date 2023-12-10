#ifndef TREES_HEADER
#define TREES_HEADER

#include "TextParse/text_parse.h"
#include "Stack/stack.h"

typedef char* TreeDataType_t;

typedef double NumType_t;

typedef enum
{
    kAdd  = 0,
    kSub  = 1,
    kMult = 2,
    kDiv  = 3,
    kSqrt = 4,
    kSin  = 5,
    kCos  = 6,
    kTg   = 7,
    kLn   = 8,
    kExp  = 9,
    kNotAnOperation,
} OpCode_t;

typedef enum
{
    kOperator,
    kConstNumber,
    kVariable,
    kRepVar,
} ExpressionType_t;

typedef enum
{
    kTreeSuccess,
    kFailedAllocation,
    kFailedToOpenFile,
    kFailedToFind,
    kFailedToReadText,
    kFailedToReadTree,
    kTreeOptimized,
    kTreeNotOptimized,
} TreeErrs_t;

typedef enum
{
    kChanged,
    kNotChanged,
} Changes_t; // bool

union NodeData
{
    NumType_t const_val;
    OpCode_t op_code;
    size_t variable_pos;
};

struct TreeNode
{
    NodeData data;

    ExpressionType_t type;

    TreeNode *parent;
    TreeNode *left;
    TreeNode *right;
};

struct Tree
{
    TreeNode *root;

    Changes_t status; // ????
};


static const char kGoLeft  = 0; // enum
static const char kGoRight = 1;


TreeErrs_t TreeVerify(Tree *tree);

TreeErrs_t TreeCtor(Tree *tree);

TreeErrs_t s_NodeCtor(Tree                  *tree,
                      TreeNode              *parent_node,
                      TreeNode             **node,
                      const TreeDataType_t   node_val);

TreeNode *NodeCtor(TreeNode         *parent_node,
                   TreeNode         *left,
                   TreeNode         *right,
                   ExpressionType_t  type,
                   double            data);

TreeErrs_t TreeDtor(TreeNode *root);

TreeErrs_t PrintTreeInFile(Tree       *tree,
                           const char *file_name);

TreeErrs_t ReadTreeOutOfFile(Tree       *tree,
                             const char *file_name);

TreeErrs_t SwapNodesData(TreeNode *node_lhs,
                         TreeNode *node_rhs);

TreeNode *FindNode(Stack          *stk,
                   TreeNode       *node,
                   TreeDataType_t  key);

TreeNode *CopyNode(const TreeNode *src_node,
                   TreeNode       *parent_node);

TreeErrs_t SetParents(TreeNode *parent_node);

TreeErrs_t GetDepth(const TreeNode *node, int *depth);

#endif
