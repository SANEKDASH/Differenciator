#include <stdio.h>
#include <stdlib.h>

#include "trees.h"
#include "diff.h"
#include "tree_dump.h"
#include "parse.h"

int main()
{
    InitTreeGraphDump();
    char *end = nullptr;

    Tree huy = {0};
    huy.root = GetG("x^x");

    GRAPH_DUMP_TREE(&huy);

    Tree differed_tree = {0};
    differed_tree.root = DiffTree(huy.root, nullptr);
    LatexDump(differed_tree.root, "latex.tex");
    SetParents(differed_tree.root);

    GRAPH_DUMP_TREE(&differed_tree);
    Variable var = {3};


    double a = Eval(differed_tree.root, var);
    printf("result - %lg\n", a);

    TreeDtor(huy.root);
    EndTreeGraphDump();

    return 0;
}
