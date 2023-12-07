#include <stdio.h>
#include <ctype.h>

#include "parse.h"
#include "trees.h"


static const char *output_file_name = "TOP_G_DUMP.txt";
static FILE *output_file = nullptr;
static const char *string = nullptr;
static int         pos    = 0;

#define LOG_PRINT(...) fprintf(output_file, __VA_ARGS__);

static void SkipSpaces();

TreeNode *GetG(const char *str)
{
    output_file = fopen(output_file_name, "w");

    LOG_PRINT("im GetG leading all work\n\n");

    string = str;
    pos    = 0;

    TreeNode *node = GetE();

    SkipSpaces();

    if (string[pos] != '\0')
    {
        printf("GetG() syntax error pos %d, string %s\n", pos, string + pos);

        return nullptr;
    }

    SetParents(node);

    fclose(output_file);

    LOG_PRINT("GetG finished work\n");

    return node;
}

TreeNode *GetP()
{
    LOG_PRINT("i'm getP reading '(' and ')' on pos %d\n\t%s\n\n", pos, string + pos);

    TreeNode *node = 0;

    SkipSpaces();

    if (string[pos] == '(')
    {
        pos++;

        node = GetE();

        SkipSpaces();

        if (string[pos] != ')')
        {
            printf("GetP() syntax error pos %d, string %s\n", pos, string + pos);
        }

        ++pos;

        return node;
    }
    else
    {
        return GetA();
    }
}

TreeNode *GetT()
{
    LOG_PRINT("i'm getT reading '*' and '/' on pos %d\n\t%s\n\n", pos, string + pos);

    TreeNode* node_lhs = GetP();

    while (string[pos] == '*' || string[pos] == '/' || string[pos] == '^')
    {
        char op = string[pos];

        pos++;

        TreeNode *node_rhs = GetP();

        switch (op)
        {
            case '*':
            {
                node_lhs =  NodeCtor(nullptr,
                                     node_lhs,
                                     node_rhs,
                                     kOperator,
                                     kMult);

                break;
            }

            case '/':
            {
                node_lhs = NodeCtor(nullptr,
                                    node_lhs,
                                    node_rhs,
                                    kOperator,
                                    kDiv);

                break;
            }

            case '^':
            {
                node_lhs = NodeCtor(nullptr,
                                    node_lhs,
                                    node_rhs,
                                    kOperator,
                                    kExp);
                break;
            }

            default:
            {
                printf("GetT() syntax error pos %d, string %s\n", pos, string + pos);
            }
        }
    }

    return node_lhs;
}

TreeNode *GetA()
{
    LOG_PRINT("Im GetA[rgument] reading args on pos %d,\n\t string %s\n\n", pos, string + pos);
    SkipSpaces();

    if (isdigit(string[pos]) || string[pos] == '-')
    {
        return GetN();
    }
    else
    {
        return GetId();
    }
}

TreeNode *GetE()
{
    LOG_PRINT("i'm getE reading '+' and '-' op pos %d\n\t%s\n\n", pos, string + pos);

    TreeNode *node_lhs = GetT();

    while (string[pos] == '+' || string[pos] == '-')
    {
        char op = string[pos];

        pos++;

        TreeNode *node_rhs = GetT();

        switch (op)
        {
            case '+':
            {
                node_lhs = NodeCtor(nullptr,
                                    node_lhs,
                                    node_rhs,
                                    kOperator,
                                    kAdd);

                break;
            }

            case '-':
            {
                node_lhs = NodeCtor(nullptr,
                                    node_lhs,
                                    node_rhs,
                                    kOperator,
                                    kSub);

                break;
            }

            default:
            {
                printf("GetE() syntax error pos %d, string %s\n", pos, string + pos);
            }
        }
    }

    return node_lhs;
}

TreeNode* GetN()
{
    LOG_PRINT("i'm getN reading numbers on pos %d\n\t%s\n\n", pos, string + pos);

    SkipSpaces();

    int val = 0;
    size_t old_pos = pos;
    char *num_end = nullptr;

    val = strtod(string + pos, &num_end);

    pos += num_end - string - pos;

    if (pos <= old_pos)
    {
        printf("GetN() syntax error pos %d, string %s\n", pos, string + pos);
    }

    SkipSpaces();

    return NodeCtor(nullptr,
                    nullptr,
                    nullptr,
                    kConstNumber,
                    val);
}

TreeNode *GetId()
{
    LOG_PRINT("Im GetV reading variables on pos %d,\n\t string : %s\n\n", pos, string + pos);

    SkipSpaces();

    if (strncmp(string + pos, "cos(", 4) == 0)
    {
        pos += 3;

        return NodeCtor(nullptr,
                        nullptr,
                        GetP(),
                        kOperator,
                        kCos);
    }
    else if (strncmp(string + pos, "sin(", 4) == 0)
    {
        pos += 3;

        return NodeCtor(nullptr,
                        nullptr,
                        GetP(),
                        kOperator,
                        kSin);
    }
    else if (strncmp(string + pos, "ln(", 3) == 0)
    {
        pos += 2;

        return NodeCtor(nullptr,
                        nullptr,
                        GetP(),
                        kOperator,
                        kLn);
    }
    else if (strncmp(string + pos, "tg(", 3) == 0)
    {
        pos += 2;

        return NodeCtor(nullptr,
                        nullptr,
                        GetP(),
                        kOperator,
                        kTg);
    }


    while (isalpha(string[pos]))
    {
        ++pos;
    }

    SkipSpaces();

    return NodeCtor(nullptr,//check null str
                    nullptr,
                    nullptr,
                    kVariable,
                    0);
}

static void SkipSpaces()
{
    while (isspace(string[pos]))
    {
        pos++;
    }
}
