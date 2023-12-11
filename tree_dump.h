#ifndef TREE_DUMP_HEADER
#define TREE_DUMP_HEADER

#include "trees.h"
#include "parse.h"

#ifdef DEBUG
#define GRAPH_DUMP_TREE(tree) GraphDumpTree(tree, __FILE__, __func__, __LINE__)
#else
#define GRAPH_DUMP_TREE ;
#endif

struct Replace
{
    TreeNode *node;
    char id;
};

struct Replaces
{
    size_t curr_depth = 0;
    Replace *rep_array = nullptr;
    size_t rep_count = 0;
};


TreeErrs_t GraphDumpTree(Tree *tree,
                         const char *file,
                         const char *func,
                         const int line);

void EndTreeGraphDump();

void InitTreeGraphDump();

void LogPrintEdges(TreeNode *node,
                   FILE     *dot_file);

void LatexDump(Variables      *vars,
               const Tree     *func,
               Expr           *expr,
               const char     *latex_file_name);

TreeErrs_t LatexPrintNode(Replaces       *reps,
                          Variables      *vars,
                          const TreeNode *node,
                          FILE           *latex_file);

TreeErrs_t PrintMaclaurinSeries(Variables  *vars,
                                const Tree *func,
                                FILE       *latex_file,
                                Expr       *expr);

void InFixPrintTree(Variables *vars,
                    TreeNode  *node,
                    FILE      *output_file);

void MakeGraph(Expr       *expr,
               const char *output_file_name);

#endif
