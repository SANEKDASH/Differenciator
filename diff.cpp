#include <string.h>
#include <math.h>
#include <tgmath.h>

#include "trees.h"
#include "diff.h"
#include "debug/debug.h"
#include "parse.h"

#define NUM_CTOR(num) NodeCtor(nullptr, nullptr, nullptr, kConstNumber, num)

OpCode_t SeekOperator(const char *op_str)
{
    CHECK(op_str);

    for (size_t i = 0; i < kOperationCount; i++)
    {
        if (strcmp(op_str, OperationArray[i].op_str) == 0)
        {
            return OperationArray[i].op_code;
        }
    }

    return kNotAnOperation;
}

//==============================================================================

DiffErrs_t SetNumber(TreeNode   *node,
                     const char *num_str)
{
    CHECK(node);
    CHECK(num_str);

    char *end_of_num = nullptr;

    node->data.const_val = strtod(num_str, &end_of_num);

    if (*end_of_num != '\0')
    {
        return kNotANumber;
    }

    return kDiffSuccess;
}

//==============================================================================

DiffErrs_t SetUpData(TreeNode   *node,
                     const char *str)
{
    CHECK(node);
    CHECK(str);

    OpCode_t op_code = kNotAnOperation;

    if ((op_code = SeekOperator(str)) != kNotAnOperation)
    {
        node->type = kOperator;

        node->data.op_code = op_code;

        return kDiffSuccess;
    }

    if (SetNumber(node, str) != kNotANumber)
    {
        node->type = kConstNumber;

        return kDiffSuccess;
    }

    printf(">> SetUpData() unknown op_code. str = %s\n", str);

    return kSyntaxError;
}

//==============================================================================

NumType_t Eval(const TreeNode *node, Variable var)
{
    if (node == nullptr)
    {
        return 0;
    }

    if (node->type == kConstNumber)
    {
        return node->data.const_val;
    }

    if (node->type == kVariable)
    {
        return var.value;
    }

    NumType_t left  = Eval(node->left, var);
    NumType_t right = Eval(node->right, var);

    switch (node->data.op_code)
    {
        case kAdd:
        {
            return left + right;
        }

        case kSub:
        {
            return left - right;
        }

        case kMult:
        {
            return left * right;
        }

        case kDiv:
        {
            return left / right;
        }

        case kSqrt:
        {
            return sqrt(right);
        }

        case kSin:
        {
            return sin(right);
        }

        case kCos:
        {
            return cos(right);
        }

        case kTg:
        {
            return tan(right);
        }

        case kExp:
        {
            return pow(left, right);
        }

        case kLn:
        {
            return log(right);
        }

        default:
        {
            printf("Eval() got unknown op_code.");

            break;
        }
    }

    return NAN;
}

//==============================================================================

TreeNode *DiffTree(const TreeNode *node,
                   TreeNode       *parent_node)
{
    #define D(node) DiffTree(node, nullptr)
    #define C(node) CopyNode(node, nullptr)

    CHECK(node);

    if (node->type == kConstNumber)
    {
        return NodeCtor(parent_node, nullptr, nullptr, kConstNumber, 0);
    }

    if (node->type == kVariable)
    {
        return NodeCtor(parent_node, nullptr, nullptr, kConstNumber, 1);
    }

    switch (node->data.op_code)
    {
        case kAdd:
        {
            return NodeCtor(parent_node,
                            D(node->left),
                            D(node->right),
                            kOperator,
                            kAdd);

            break;
        }

        case kSub:
        {
            return NodeCtor(parent_node,
                            D(node->left),
                            D(node->right),
                            kOperator,
                            kSub);

            break;
        }

        case kMult:
        {
                return NodeCtor(node->parent,
                                NodeCtor(nullptr,
                                         D(node->left),
                                         C(node->right),
                                         kOperator,
                                         kMult),
                                NodeCtor(nullptr,
                                         C(node->left),
                                         D(node->right),
                                         kOperator,
                                         kMult),
                                kOperator,
                                kAdd);

            break;
        }

        case kDiv:
        {
            return NodeCtor(node->parent,
                            NodeCtor(nullptr,
                                     NodeCtor(nullptr,
                                              D(node->left),
                                              C(node->right),
                                              kOperator,
                                              kMult),
                                     NodeCtor(nullptr,
                                              C(node->left),
                                              D(node->right),
                                              kOperator,
                                              kMult),
                                     kOperator,
                                     kSub),
                             NodeCtor(nullptr,
                                      C(node->right),
                                      NUM_CTOR(2),
                                      kOperator,
                                      kExp),
                            kOperator,
                            kDiv);

            break;

        }

        case kCos:
        {
            return NodeCtor(parent_node,
                            NodeCtor(nullptr,
                                     NUM_CTOR(-1),
                                     NodeCtor(nullptr,
                                              nullptr,
                                              C(node->right),
                                              kOperator,
                                              kSin),
                                     kOperator,
                                     kMult),
                            D(node->right),
                            kOperator,
                            kMult);
        }

        case kSin:
        {
            return NodeCtor(parent_node,
                            NodeCtor(nullptr,
                                     nullptr,
                                     C(node->right),
                                     kOperator,
                                     kCos),
                            D(node->right),
                            kOperator,
                            kMult);
        }

        case kTg:
        {
            return NodeCtor(parent_node,
                            NodeCtor(nullptr,
                                     NUM_CTOR(1),
                                     NodeCtor(nullptr,
                                              NodeCtor(nullptr,
                                                       nullptr,
                                                       C(node->right),
                                                       kOperator,
                                                       kCos),
                                              NUM_CTOR(2),
                                              kOperator,
                                              kExp),
                                     kOperator,
                                     kDiv),
                            D(node->right),
                            kOperator,
                            kMult);
        }

        case kLn:
        {
            return NodeCtor(parent_node,
                            NodeCtor(nullptr,
                                     NUM_CTOR(1),
                                     C(node->right),
                                     kOperator,
                                     kDiv),
                            D(node->right),
                            kOperator,
                            kMult);
        }

        case kExp:
        {
            return NodeCtor(parent_node,
                            C(node),
                            NodeCtor(nullptr,
                                     NodeCtor(nullptr,
                                              NodeCtor(nullptr,
                                                       C(node->right),
                                                       C(node->left),
                                                       kOperator,
                                                       kDiv),
                                              D(node->left),
                                              kOperator,
                                              kMult),
                                     NodeCtor(nullptr,
                                              D(node->right),
                                              NodeCtor(nullptr,
                                                       nullptr,
                                                       C(node->left),
                                                       kOperator,
                                                       kLn),
                                              kOperator,
                                              kMult),
                                     kOperator,
                                     kAdd),
                            kOperator,
                            kMult);
        }

        default:
        {
            printf("Diff() don't know such derivative OPCODE : %d, TYPE: %d\n", node->data.op_code, node->type);

            break;
        }
    }

    return nullptr;
}

//==============================================================================
