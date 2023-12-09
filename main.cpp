#include <stdio.h>
#include <stdlib.h>

#include "trees.h"
#include "diff.h"
#include "tree_dump.h"
#include "parse.h"

int main(int argc, const char *argv[])
{

    InitTreeGraphDump();
    Expr expr;
    Variables vars;

    VarArrayInit(&vars);

    expr.string = "cos(x * y)";
    expr.pos  = 0;
    Tree func = {0};

    func.root = GetG(&vars, &expr); // file
    OptimizeTree(&vars, &func);
    GRAPH_DUMP_TREE(&func);

    LatexDump(&vars, &func, "latex.tex"); // argv

    VarArrayDtor(&vars);
    TreeDtor(func.root);
    printf("HUY");
    EndTreeGraphDump();

    return 0;
}
