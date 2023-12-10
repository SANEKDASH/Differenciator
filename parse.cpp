#include <stdio.h>
#include <ctype.h>

#include "parse.h"
#include "trees.h"


static const char *output_file_name = "TOP_G_DUMP.txt";
static FILE *output_file = nullptr;


#define LOG_PRINT(...) fprintf(output_file, __VA_ARGS__);

static void SkipSpaces(Expr *expr);

static const size_t kBaseVarCount = 16;

static const size_t kMaxIdLen = 64;

int VarArrayInit(Variables *vars)
{
    vars->var_array = (Variable *) calloc(kBaseVarCount, sizeof(Variable));

    if (vars->var_array == nullptr)
    {
        return -1;
    }

    vars->size = kBaseVarCount;

    vars->var_count = 0;

    return 0;
}

int SeekVariable(Variables *vars, const char *var_name)
{
    for (size_t i = 0; i < vars->var_count; i++)
    {
        if (strcmp(var_name, vars->var_array[i].id) == 0)
        {
            return i;
        }
    }

    return -1;
}

int AddVar(Variables *vars, const char *var_name)
{
    vars->var_array[vars->var_count].id    = strdup(var_name);
    vars->var_array[vars->var_count].value = 0;

    ++vars->var_count;
    return vars->var_count;
}

int VarArrayDtor(Variables *vars)
{
    free(vars->var_array);
    vars->var_array = nullptr;

    vars->size = vars->var_count = 0;

    return 0;
}


TreeNode *GetG(Variables *vars, Expr *expr) // !!!! rename
{
    #define CUR_CHAR expr->string[expr->pos]
    #define STRING   expr->string
    #define POS      expr->pos

    output_file = fopen(output_file_name, "w");
    LOG_PRINT("im GetG leading all work\n\n");

    TreeNode *node = GetE(vars, expr);

    SkipSpaces(expr);

    if (CUR_CHAR != '\0')
    {
        printf("GetG() syntax error pos %d, string %s\n", POS, STRING + POS);

        return nullptr;
    }

    SetParents(node);

    fclose(output_file);

    LOG_PRINT("GetG finished work\n");

    return node;
}

TreeNode *GetP(Variables *vars, Expr *expr)
{
    LOG_PRINT("i'm getP reading '(' and ')' on pos %d\n\t%s\n\n", POS, STRING+ POS);

    TreeNode *node = 0;

    SkipSpaces(expr);

    if (CUR_CHAR == '(')
    {
        POS++;

        node = GetE(vars, expr);

        SkipSpaces(expr);

        if (CUR_CHAR != ')')
        {
            printf("GetP() syntax error pos %d, string %s\n", POS, STRING+ POS);
        }

        ++POS;

        return node;
    }
    else
    {
        return GetA(vars, expr);
    }
}

TreeNode *GetT(Variables *vars, Expr *expr)
{
    LOG_PRINT("i'm getT reading '*' and '/' on pos %d\n\t%s\n\n", POS, STRING+ POS);

    TreeNode* node_lhs = GetP(vars, expr);

    while (CUR_CHAR == '*' || CUR_CHAR == '/' || CUR_CHAR == '^')
    {
        char op = CUR_CHAR;

        POS++;

        TreeNode *node_rhs = GetP(vars, expr);

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
                printf("GetT() syntax error pos %d, string %s\n", POS, STRING+ POS);
            }
        }
    }

    return node_lhs;
}

TreeNode *GetA(Variables *vars, Expr *expr)
{
    LOG_PRINT("Im GetA[rgument] reading args on pos %d,\n\t string %s\n\n", POS, STRING+ POS);
    SkipSpaces(expr);

    if (isdigit(CUR_CHAR) || CUR_CHAR == '-')
    {
        return GetN(vars, expr);
    }
    else
    {
        return GetId(vars, expr);
    }
}

TreeNode *GetE(Variables *vars, Expr *expr)
{
    LOG_PRINT("i'm getE reading '+' and '-' op pos %d\n\t%s\n\n", POS, STRING+ POS);

    TreeNode *node_lhs = GetT(vars, expr);

    while (CUR_CHAR == '+' || CUR_CHAR == '-')
    {
        char op = CUR_CHAR;

        POS++;

        TreeNode *node_rhs = GetT(vars, expr);

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
                printf("GetE() syntax error pos %d, string %s\n", POS, STRING+ POS);
            }
        }
    }

    return node_lhs;
}

TreeNode* GetN(Variables *vars, Expr *expr)
{
    LOG_PRINT("i'm getN reading numbers on pos %d\n\t%s\n\n", POS, STRING+ POS);

    SkipSpaces(expr);

    int val = 0;
    size_t old_pos = POS;
    char *num_end = nullptr;

    val = strtod(STRING+ POS, &num_end);

    POS = num_end - STRING;

    if (POS <= old_pos)
    {
        printf("GetN() syntax error pos %d, string %s\n", POS, STRING + POS);
    }

    SkipSpaces(expr);

    return NodeCtor(nullptr,
                    nullptr,
                    nullptr,
                    kConstNumber,
                    val);
}

TreeNode *GetId(Variables *vars, Expr *expr)
{
    LOG_PRINT("Im GetV reading variables on pos %d,\n\t string : %s\n\n", POS, STRING + POS);

    SkipSpaces(expr);

    if (strncmp(STRING + POS, "cos(", 4) == 0)
    {
        POS += 3;

        return NodeCtor(nullptr,
                        nullptr,
                        GetP(vars, expr),
                        kOperator,
                        kCos);
    }
    else if (strncmp(STRING + POS, "sin(", 4) == 0)
    {
        POS += 3;

        return NodeCtor(nullptr,
                        nullptr,
                        GetP(vars, expr),
                        kOperator,
                        kSin);
    }
    else if (strncmp(STRING + POS, "ln(", 3) == 0)
    {
        POS += 2;

        return NodeCtor(nullptr,
                        nullptr,
                        GetP(vars, expr),
                        kOperator,
                        kLn);
    }
    else if (strncmp(STRING + POS, "tg(", 3) == 0)
    {
        POS += 2;

        return NodeCtor(nullptr,
                        nullptr,
                        GetP(vars, expr),
                        kOperator,
                        kTg);
    }

    static char var_name[kMaxIdLen] = {0};
    var_name[0] = '\0';
    size_t i = 0;

    while (isalpha(CUR_CHAR))
    {
        var_name[i++] = CUR_CHAR;
        ++POS;
    }
    var_name[i] = '\0';

    int var_pos = SeekVariable(vars, var_name);
    if (var_pos < 0)
    {
        var_pos = AddVar(vars, var_name);

    }

    SkipSpaces(expr);

    return NodeCtor(nullptr,//check null str
                    nullptr,
                    nullptr,
                    kVariable,
                    var_pos);
}

static void SkipSpaces(Expr *expr)
{
    while (isspace(CUR_CHAR))
    {
        POS++;
    }
}
