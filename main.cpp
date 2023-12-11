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


    const char *output_file_name = "latex.tex";

    VarArrayInit(&vars);

    if (argc >= 2)
    {
        expr.string = "1/x";
    }
    else
    {
        printf(">>You must type a function you want to diff.");

        return -1;
    }

    expr.pos  = 0;
    Tree func = {0};


    func.root = GetG(&vars, &expr);
    OptimizeTree(&vars, &func);

    LatexDump(&vars, &func, &expr, output_file_name);

    EndTreeGraphDump();

    GRAPH_DUMP_TREE(&func);
    TreeDtor(func.root);
    VarArrayDtor(&vars);

    return 0;
}
