#ifndef TREE_DUMP_HEADER
#define TREE_DUMP_HEADER

#include "trees.h"

#ifdef DEBUG
#define GRAPH_DUMP_TREE(tree) GraphDumpTree(tree)
#else
#define GRAPH_DUMP_TREE ;
#endif

TreeErrs_t GraphDumpTree(Tree *tree);

void EndTreeGraphDump();

void InitTreeGraphDump();

void LogPrintEdges(TreeNode *node,
                   FILE     *dot_file);

void LatexDump(const TreeNode *node,
               const char     *latex_file_name);

TreeErrs_t LatexPrintNode(const TreeNode *node,
                          FILE           *latex_file);

#endif
