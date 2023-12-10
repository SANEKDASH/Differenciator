#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "debug/debug.h"
#include "trees.h"
#include "tree_dump.h"
#include "diff.h"
#include "time.h"

#define SVG

static size_t call_count = 0;

static FILE *log_file = nullptr;

static const char * const log_file_name = "tree.dmp.html";

static void LogPrintTree(TreeNode *node,
                         FILE     *dot_file);

static void PrintWithBrackets(Replaces *reps,
                              Variables *vars,
                              TreeNode *node,
                              FILE *latex_file);


static int Factorial(int num);

static const char *FoolStrings[] =
{
    "-Саня ты че творишь?\\newline\n-Это рофлс!",
    "Сунул в попу я огурчик, выпил скотч и пукнул в стульчик...\\newline\nA потом получил:",
    "Методом введения ЛСД в вену получили:",
    "Округлим до прямой:",
    "Очевидно, что:"
};

static const size_t kFoolStringsSize = sizeof(FoolStrings) / sizeof(char *);

static TreeErrs_t RepCtor(Replaces *reps);

static TreeErrs_t RepDtor(Replaces *reps);

static TreeErrs_t MakeReplace(Replaces  *reps,
                              size_t    *depth,
                              TreeNode **node);

static size_t AddReplace(Replaces *reps,
                         TreeNode *node);

static void PasteImage(FILE       *latex_file,
                       const char *image_name);

static void PrintReps(Replaces *reps,
                      Variables *vars,
                      FILE *latex_file);

static const char *transpos_latex_string = "\\makeatletter\n"
                                            "\\newenvironment{wrapeqn}[2][.9\\displaywidth]\n"
                                            "{\\begin{minipage}{#1}\\openup\\jot\\change@operations\n"
                                            "\\@hangfrom{$\\displaystyle#2{}$}$\\displaystyle}\n"
                                            "{$\\end{minipage}}\n"
                                            "\\newcommand{\\change@operations}{%\n"
                                            "\\begingroup\\lccode`~=`+\\lowercase{\\endgroup\\let~}\\prebin@plus\n"
                                            "\\begingroup\\lccode`~=`-\\lowercase{\\endgroup\\let~}\\prebin@minus\n"
                                            "\\mathcode`+=\"8000 \\mathcode`-=\"8000\n"
                                            "}\n"
                                            "\\edef\\prebin@plus{\\penalty\\binoppenalty\\mathchar\\the\\mathcode`+\\noexpand\\nobreak}\n"
                                            "\\edef\\prebin@minus{\\penalty\\binoppenalty\\mathchar\\the\\mathcode`-\\noexpand\\nobreak}\n"
                                            "\\makeatother\n";

//================================================================================================

void InitTreeGraphDump()
{
    system("rm -f *.svg");
    system("rm -f *.png");

    log_file = fopen(log_file_name, "w");

    if (log_file == nullptr)
    {
        perror("BeginListGraphDump() failed to open file");

        return;
    }

    fprintf(log_file, "<pre>\n");
}

//================================================================================================

void EndTreeGraphDump()
{
    if (log_file == nullptr)
    {
        perror("EndLostGrapDump() failed to write an info");

        return;
    }

    fclose(log_file);
}

//================================================================================================

TreeErrs_t GraphDumpTree(Tree *tree,
                         const char *file,
                         const char *func,
                         const int line)
{
    FILE *dot_file = fopen("tree.dmp.dot", "w");

    #define LOG_PRINT(...) fprintf(dot_file, __VA_ARGS__)

    if (dot_file == nullptr)
    {
        perror("GraphDumpList() failed to open dump file");

        return kFailedToOpenFile;
    }

    static char cmd_command[200] = {0};

    assert(dot_file);

    LOG_PRINT("digraph List\n{\n"
              "\trankdir = TB;\n"
              "\tgraph [bgcolor = \"black\"]\n"
              "\tnode[color =\"black\", fontsize=14, shape = Mrecord];\n"
              "\tedge[color = \"red\", fontcolor = \"blue\",fontsize = 12];\n\n\n");


    LogPrintTree(tree->root, dot_file);

    LogPrintEdges(tree->root, dot_file);


    LOG_PRINT("\n\n}");

    fclose(dot_file);

    //system("iconv -f CP1251 -t UTF-8 tree.dmp.dot > ctree.dmp.dot");

    sprintf(cmd_command, "dot -Tsvg tree.dmp.dot -o graphdump%d.svg"
                         , call_count);

    system(cmd_command);


    fprintf(log_file, "DATE : %s \nTIME : %s\n"
                      "Called from file: %s\n"
                      "Called from function: %s\n"
                      "Line: %d\n"
                      "<img height=\"150px\" src=\"graphdump%d.svg\">\n"
                      "-----------------------------------------------------------------\n",
                      __DATE__,
                      __TIME__,
                      file,
                      func,
                      line,
                      call_count);


    ++call_count;

    return kTreeSuccess;
}

//================================================================================================

static void LogPrintTree(TreeNode *node,
                         FILE     *dot_file)
{
    if (node->type == kOperator)
    {
        LOG_PRINT("node%p [style = filled, fillcolor = \"lightgreen\", shape = Mrecord, label = "
                  "\"data: %s | {type : operator | op_code : %d} | {parent: %p | pointer: %p | left: %p | right: %p} \"]\n",
                  node,
                  OperationArray[node->data.op_code].op_str,
                  node->data.op_code,
                  node->parent,
                  node,
                  node->left,
                  node->right);
    }
    else if (node->type == kConstNumber)
    {
        LOG_PRINT("node%p [style = filled, fillcolor = \"lightblue\", shape = Mrecord, label = "
                  "\"data: %lg | type : const number | {parent: %p | pointer: %p | left: %p | right: %p} \"]\n",
                  node,
                  node->data.const_val,
                  node->parent,
                  node,
                  node->left,
                  node->right);
    }
    else if (node->type == kVariable)
    {
        LOG_PRINT("node%p [style = filled, fillcolor = \"pink\", shape = Mrecord, label = "
                  "\"data: %d | type : variable | {parent: %p | pointer: %p | left: %p | right: %p} \"]\n",
                  node,
                  node->data.variable_pos,
                  node->parent,
                  node,
                  node->left,
                  node->right);
    }

    if (node->left != nullptr)
    {
        LogPrintTree(node->left, dot_file);
    }

    if (node->right != nullptr)
    {
        LogPrintTree(node->right, dot_file);
    }
}

//================================================================================================

void LogPrintEdges(TreeNode *node,
                   FILE     *dot_file)
{
    if (node->left != nullptr)
    {
        LOG_PRINT("node%p->node%p\n",
                  node,
                  node->left);
    }

    if (node->parent != nullptr)
    {
        LOG_PRINT("node%p->node%p[color = \"yellow\"]\n",
                  node,
                  node->parent);
    }

    if (node->right != nullptr)
    {
        LOG_PRINT("node%p->node%p\n",
                  node,
                  node->right);
    }

    if (node->left != nullptr)
    {
        LogPrintEdges(node->left, dot_file);
    }

    if (node->right != nullptr)
    {
        LogPrintEdges(node->right, dot_file);
    }
}

#undef LOG_PRINT

//================================================================================================

void LatexDump(Variables      *vars,
               Tree           *func,
               const char     *latex_file_name)
{
    system("rm -rf *.pdf");
    system("rm -rf *.log");
    system("rm -fr *.aux");


    FILE *latex_file = fopen(latex_file_name, "w");
    static const size_t kMaxCmdLen = 128;

    static char system_cmd[kMaxCmdLen] = {0};

    if (latex_file == nullptr)
    {
        perror("LatexDump() failed to open file");

        return;
    }


    #define TEX_PRINT(...) fprintf(latex_file, __VA_ARGS__)

    TEX_PRINT("\\documentclass[a4paper,14pt]{extarticle}\n"
              "\\usepackage{graphicx}\n"
              "\\usepackage{ucs}\n"
              "\\usepackage[utf8x]{inputenc}\n"
              "\\usepackage[russian]{babel}\n"
              "\\usepackage{multirow}\n"
              "\\usepackage{mathtext}\n"
              "\\usepackage[T2A]{fontenc}\n"
              "\\usepackage{titlesec}\n"
              "\\usepackage{float}\n"
              "\\usepackage{empheq}\n"
              "\\usepackage{amsfonts}\n"
              "\\usepackage{amsmath}\n"
              "%s\n\\title{Лабораторная работа номер 2.2.8}\n"
              "\\begin{document}\n"
              "\\maketitle\n", transpos_latex_string);

    PrintMaclaurinSeries(vars, func, latex_file);
    printf("HUY");

    TEX_PRINT("\n\\end{document}");

    fclose(latex_file);
    sprintf(system_cmd, "iconv -f CP1251 -t UTF-8 %s > c%s", latex_file_name, latex_file_name);
    system(system_cmd);

    sprintf(system_cmd, "pdflatex -interaction=batchmode -halt-on-error -file-line-error c%s", latex_file_name);
    system(system_cmd);
}

//================================================================================================

TreeErrs_t LatexPrintNode(Replaces       *reps,
                          Variables      *vars,
                          const TreeNode *node,
                          FILE           *latex_file)
{
    #define PRINT_BR(node) PrintWithBrackets(reps, vars, node, latex_file)

    /*if (reps != nullptr)
    {
        ++(reps->curr_depth);

        if (reps->curr_depth >= 32 && reps->rep_count < 14)
        {
            size_t pos = AddReplace(reps, (TreeNode *) node);
            TEX_PRINT("%C", reps->rep_array[pos].id);
            reps->curr_depth = 0;

            return kTreeSuccess;
        }
    }*/

    if (node == nullptr)
    {
        return kTreeSuccess;
    }

    if (node->type == kConstNumber)
    {
        TEX_PRINT("%lg", node->data.const_val);

        return kTreeSuccess;
    }

    if (node->type == kVariable)
    {
        TEX_PRINT("%s", vars->var_array[node->data.variable_pos].id);

        return kTreeSuccess;
    }

    if (node->type == kRepVar)
    {
        if (reps != nullptr)
        {
            TEX_PRINT("%c", reps->rep_array[node->data.variable_pos].id);
        }
    }

    switch (node->data.op_code)
    {
        case kAdd:
        {
            LatexPrintNode(reps, vars, node->left, latex_file);

            TEX_PRINT("+");

            LatexPrintNode(reps, vars, node->right, latex_file);

            break;
        }

        case kSub:
        {
            LatexPrintNode(reps, vars, node->left, latex_file);

            TEX_PRINT("-");

            LatexPrintNode(reps, vars, node->right, latex_file);

            break;
        }

        case kDiv:
        {
            TEX_PRINT("%s{", OperationArray[kDiv].tex_str);
            LatexPrintNode(reps, vars, node->left, latex_file);
            TEX_PRINT("}");

            TEX_PRINT("{");
            LatexPrintNode(reps, vars, node->right, latex_file);
            TEX_PRINT("}");

            break;
        }

        case kMult:
        {
            PRINT_BR(node->left);

            TEX_PRINT(" %s ", OperationArray[kMult].tex_str);

            PRINT_BR(node->right);

            break;
        }

        case kExp:
        {
            PRINT_BR(node->left);

            TEX_PRINT("^");

            TEX_PRINT("{");
            LatexPrintNode(reps, vars, node->right, latex_file);
            TEX_PRINT("}");

            break;
        }

        case kSin:
        case kCos:
        case kSqrt:
        case kTg:
        case kLn:
        {
            TEX_PRINT("%s ", OperationArray[node->data.op_code].tex_str);
            TEX_PRINT("{");
            PRINT_BR(node->right);
            TEX_PRINT("}");
            break;
        }

        default:
        {
            printf("kavo> OPCODE : %d?\n", node->data.op_code);

            return kTreeSuccess;//===
        }

    }

    return kTreeSuccess;
}

//================================================================================================

static void PrintWithBrackets(Replaces *reps,
                              Variables *vars,
                              TreeNode *node,
                              FILE *latex_file)
{
    if(node->type != kVariable && node->type != kConstNumber)
    {
        if (!IsUnaryOp(node->data.op_code) && node->data.op_code != kMult)
        {
            TEX_PRINT("\\left( ");
        }
    }

    LatexPrintNode(reps, vars, node, latex_file);

    if(node->type != kVariable && node->type != kConstNumber)
    {
        if (!IsUnaryOp(node->data.op_code) && node->data.op_code != kMult)
        {
            TEX_PRINT(" \\right)");
        }
    }
}

//================================================================================================

TreeErrs_t PrintMaclaurinSeries(Variables *vars,
                                Tree      *func,
                                FILE      *latex_file)
{

    PasteImage(latex_file, "fun_img/img1.jpg");
    srand(time(NULL));
    static const size_t kPrecise = 5;

    Tree diff_tree = {0};
    diff_tree.root = func->root;

    double coeffs[kPrecise] = {0};

    coeffs[0] = Eval(vars, diff_tree.root);

    TEX_PRINT("\\begin{equation*}\n\\begin{wrapeqn}\n f(x) = ");
    LatexPrintNode(nullptr, vars, diff_tree.root, latex_file);
    TEX_PRINT("\\end{wrapeqn}\n\\end{equation*}\n");

    for (size_t i = 1; i < kPrecise; i++)
    {
        Replaces reps;
        RepCtor(&reps);

        TreeNode *tmp = diff_tree.root;

        diff_tree.root = DiffTree(diff_tree.root, nullptr);

        TreeDtor(tmp);

        //GRAPH_DUMP_TREE(&diff_tree);
        OptimizeTree(vars, &diff_tree);
        //GRAPH_DUMP_TREE(&diff_tree);

        coeffs[i] = Eval(vars, diff_tree.root);

        GRAPH_DUMP_TREE(&diff_tree);

        TEX_PRINT("%s\\newline\n", FoolStrings[rand() % kFoolStringsSize]);

        TEX_PRINT("\\begin{equation*}\n\\begin{wrapeqn}\nf^{%d}(x) = ", i);
        LatexPrintNode(&reps, vars, diff_tree.root, latex_file);
        TEX_PRINT("\\end{wrapeqn}\n\\end{equation*}\n");

        PrintReps(&reps, vars, latex_file);

        TEX_PRINT("$$f^{%d}(0) = %lg$$", i, Eval(vars, diff_tree.root));
    }

    TEX_PRINT("Ряд Маклорена:\\newline\n$$f(x) = ");

    for (size_t i = 0; i < kPrecise; i++)
    {
        if (coeffs[i] != 0)
        {
            if (i == 0)
            {
                TEX_PRINT("\\frac{%lg}{%d} +", coeffs[i], Factorial(i));
            }
            else
            {
                TEX_PRINT("\\frac{%lg}{%d} \\cdot x^{%d} +", coeffs[i], Factorial(i), i);
            }
        }
    }

    TEX_PRINT("O(x^%d)$$", kPrecise);

    TreeDtor(diff_tree.root);
}

//================================================================================================

static void PasteImage(FILE       *latex_file,
                       const char *image_name)
{
    TEX_PRINT("\\begin{figure}[h]"
              "\\centering"
              "\\includegraphics[scale=0.5]{%s}"
              "\\caption{График функции $u^2(T)$.}"
              "\\end{figure}", image_name);
}

//================================================================================================

static int Factorial(int num)
{
    if (num == 1 || num == 0)
    {
        return 1;
    }

    return num * Factorial(num - 1);
}

//================================================================================================

static TreeErrs_t RepDtor(Replaces *reps)
{
    free(reps->rep_array);
    reps->curr_depth = 0;
    reps->rep_count = 0;

    return kTreeSuccess;
}

static TreeErrs_t RepCtor(Replaces *reps)
{
    static size_t kBaseReplaceSize = 16;

    reps->rep_array = (Replace *) calloc(kBaseReplaceSize, sizeof(Replace));

    reps->curr_depth = 0;
    reps->rep_count  = 0;

    return kTreeSuccess;
}

//================================================================================================

static size_t kSoBig = 8;

static size_t AddReplace(Replaces *reps, TreeNode *node)
{
    reps->rep_array[reps->rep_count].node = node;
    reps->rep_array[reps->rep_count].id = 'A' + reps->rep_count;

    reps->rep_count++;
    return reps->rep_count - 1;
}

//================================================================================================

static TreeErrs_t MakeReplace(Replaces *reps,
                              size_t    *depth,
                              TreeNode  **node)
{
    if ((*node)->type == kVariable ||
        (*node)->type == kConstNumber)
    {
        return kTreeSuccess;
    }

    if (*depth >= kSoBig)
    {
        size_t pos = AddReplace(reps, *node) - 1;

        *node = NodeCtor(nullptr,
                         nullptr,
                         nullptr,
                         kRepVar,
                         pos);

        return kTreeSuccess;;
    }

    *depth++;
    MakeReplace(reps, depth, &(*node)->left);

    *depth--;
    MakeReplace(reps, depth, &(*node)->right);

    return kTreeSuccess;
}

//================================================================================================

static void PrintReps(Replaces *reps,
                      Variables *vars,
                      FILE *latex_file)
{
    for(size_t i = 0; i < reps->rep_count; i++)
    {
        TEX_PRINT("$$%C = ", reps->rep_array[i].id);
        LatexPrintNode(nullptr, vars, reps->rep_array[i].node, latex_file);
        TEX_PRINT("$$\\newline\n");
    }
}

#undef PRINT_BR
#undef TEX_PRINT


