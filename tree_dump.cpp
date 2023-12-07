#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "debug/debug.h"
#include "trees.h"
#include "tree_dump.h"
#include "diff.h"

#define SVG

static size_t call_count = 0;

static FILE *log_file = nullptr;

static const char * const log_file_name = "tree.dmp.html";

static void LogPrintTree(TreeNode *node,
                         FILE     *dot_file);

static bool IsUnaryOp(const OpCode_t op_code);

static void PrintWithBrackets(TreeNode *node, FILE *latex_file);

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

TreeErrs_t GraphDumpTree(Tree *tree)
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
                      /*"Called from file: %s\n"
                      "Called from function: %s\n"
                      "Line: %d\n"*/
                      "<img height=\"150px\" src=\"graphdump%d.svg\">\n"
                      "-----------------------------------------------------------------\n",
                      /*file,
                      func,
                      line,*/
                      __DATE__,
                      __TIME__,
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
                  "\"data: x | type : variable | {parent: %p | pointer: %p | left: %p | right: %p} \"]\n",
                  node,
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

void LatexDump(const TreeNode *node,
               const char     *latex_file_name)
{
    FILE *latex_file = fopen(latex_file_name, "w");

    if (latex_file == nullptr)
    {
        perror("LatexDump() failed to open file");

        return;
    }


    #define TEX_PRINT(...) fprintf(latex_file, __VA_ARGS__)

    TEX_PRINT("\\documentclass{article}\n"
              "\\usepackage{graphicx}\n"
              "\\begin{document}\n"
              "\\title{Матан хуйни}\n"
              "\\maketitle{}\n"
              "\\date{December 2023}\n");

    TEX_PRINT("$$");

    LatexPrintNode(node, latex_file);

    TEX_PRINT("$$");
    TEX_PRINT("\n\\end{document}");

    fclose(latex_file);
}

//================================================================================================

static bool IsUnaryOp(const OpCode_t op_code)
{
    return op_code == kSqrt ||
           op_code == kSin  ||
           op_code == kCos  ||
           op_code == kTg   ||
           op_code == kLn;
}

//================================================================================================

TreeErrs_t LatexPrintNode(const TreeNode *node,
                          FILE           *latex_file)
{
    #define PRINT_BR(node) PrintWithBrackets(node, latex_file)

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
        TEX_PRINT("x");//many variables

        return kTreeSuccess;
    }

    switch (node->data.op_code)
    {
        case kAdd:
        {
            LatexPrintNode(node->left, latex_file);

            TEX_PRINT("+");

            LatexPrintNode(node->right, latex_file);

            break;
        }

        case kSub:
        {
            LatexPrintNode(node->left, latex_file);

            TEX_PRINT("-");

            LatexPrintNode(node->right, latex_file);

            break;
        }

        case kDiv:
        {
            TEX_PRINT("%s{", OperationArray[kDiv].tex_str);
            LatexPrintNode(node->left, latex_file);
            TEX_PRINT("}");

            TEX_PRINT("{");
            LatexPrintNode(node->right, latex_file);
            TEX_PRINT("}");

            break;
        }

        case kMult:
        {
            PRINT_BR(node->left);

            TEX_PRINT("%s", OperationArray[kMult].tex_str);

            PRINT_BR(node->right);

            break;
        }

        case kExp:
        {
            PRINT_BR(node->left);

            TEX_PRINT("^");

            TEX_PRINT("{");
            LatexPrintNode(node->right, latex_file);
            TEX_PRINT("}");

            break;
        }

        case kSin:
        case kCos:
        case kSqrt:
        case kTg:
        case kLn:
        {
            TEX_PRINT("%s", OperationArray[node->data.op_code].tex_str);
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

static void PrintWithBrackets(TreeNode *node, FILE *latex_file)
{
    if(node->type != kVariable && node->type != kConstNumber)
    {
        if (!IsUnaryOp(node->data.op_code))
        {
            TEX_PRINT("(");
        }
    }

    LatexPrintNode(node, latex_file);

    if(node->type != kVariable && node->type != kConstNumber)
    {
        if (!IsUnaryOp(node->data.op_code))
        {
            TEX_PRINT(")");
        }
    }
}

//================================================================================================

#undef PRINT_BR
#undef TEX_PRINT


