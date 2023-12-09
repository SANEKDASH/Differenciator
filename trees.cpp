#include <stdio.h>
#include <stdlib.h>

#include "debug/color_print.h"
#include "debug/debug.h"
#include "TextParse/text_parse.h"
#include "trees.h"
#include "tree_dump.h"
#include "Stack/stack.h"
#include "diff.h"

static const char *kTreeSaveFileName = "tree_save.txt";

static const int kPoisonVal = 0xBADBABA;

static const char *kPreCtored = "*";

static TreeErrs_t CreateNodeFromText(Tree     *tree,
                                     TreeNode *parent_node,
                                     TreeNode **curr_node,
                                     Text      *text,
                                     size_t    *iterator);

static void SeekNode(Stack           *stk,
                     TreeNode        *node,
                     TreeNode       **ret_node,
                     TreeDataType_t   key);

static TreeErrs_t CreateNodeFromBrackets(Tree      *tree,
                                         TreeNode  *parent_node,
                                         TreeNode **curr_node,
                                         Text      *text,
                                         size_t    *iterator);

static TreeErrs_t CreateNodeFromInfixText(Tree      *tree,
                                          TreeNode  *parent_node,
                                          TreeNode **curr_node,
                                          Text      *text,
                                          size_t    *iterator);

static TreeErrs_t PrintTree(const TreeNode *root,
                            FILE           *output_file);

static TreeErrs_t CreateNodeFromInfixBrackets(Tree      *tree,
                                              TreeNode  *parent_node,
                                              TreeNode **node,
                                              Text      *text,
                                              size_t    *iterator);

//==============================================================================

TreeErrs_t TreeCtor(Tree *tree)
{
    CHECK(tree);

    tree->root = nullptr;

    tree->root->left  = nullptr;
    tree->root->right = nullptr;

    tree->status = kNotChanged;

    return kTreeSuccess;
}

//==============================================================================

TreeErrs_t TreeDtor(TreeNode *root)
{
    if (root == nullptr)
    {
        return kTreeSuccess;
    }

    if (root->left != nullptr)
    {
        TreeDtor(root->left);
    }

    if (root->right != nullptr)
    {
        TreeDtor(root->right);
    }

    free(root);


    return kTreeSuccess;
}

//==============================================================================

TreeErrs_t s_NodeCtor(Tree                  *tree,
                      TreeNode              *parent_node,
                      TreeNode             **node,
                      const TreeDataType_t   node_val)
{
    CHECK(node);

    *node = (TreeNode *) calloc(1, sizeof(TreeNode));

    if (node == nullptr)
    {
        return kFailedAllocation;
    }

    SetUpData(*node, node_val);

    (*node)->left   = (*node)->right = nullptr;
    (*node)->parent = parent_node;

    GRAPH_DUMP_TREE(tree);

    return kTreeSuccess;
}

//==============================================================================

TreeNode *NodeCtor(TreeNode         *parent_node,
                   TreeNode         *left,
                   TreeNode         *right,
                   ExpressionType_t  type,
                   double            data)
{
    TreeNode *node = (TreeNode *) calloc(1, sizeof(TreeNode));

    if (node == nullptr)
    {
        return nullptr;
    }

    node->type = type;

    if (type == kOperator)
    {
        node->data.op_code = (OpCode_t) data;
    }
    else
    {
        node->data.const_val = data;
    }

    node->left  = left;
    node->right = right;
    node->parent = parent_node;

    return node;
}

//==============================================================================

static TreeErrs_t PrintTree(const TreeNode *root,
                            FILE           *output_file)
{
    CHECK(output_file);

    if (root == nullptr)
    {
        fprintf(output_file,"null ");

        return kTreeSuccess;
    }

    if (root->type != kConstNumber)
    {
        fprintf(output_file, "( ");
    }

    if (root->left != nullptr)
    {
        PrintTree(root->left, output_file);
    }

    if (root->type == kOperator)
    {
        fprintf(output_file, "%s ", OperationArray[root->data.op_code].op_str);
    }
    else if (root->type == kConstNumber)
    {
        fprintf(output_file, "%lg ", root->data.const_val);
    }

    if (root->right != nullptr)
    {
        PrintTree(root->right, output_file);
    }

    if (root->type != kConstNumber)
    {
        fprintf(output_file, ") ");
    }

    return kTreeSuccess;
}

//==============================================================================

TreeErrs_t PrintTreeInFile(Tree       *tree,
                           const char *file_name)
{
    CHECK(tree);

    FILE *output_file = fopen(file_name, "wb");

    if (output_file == nullptr)
    {
        return kFailedToOpenFile;
    }

    PrintTree(tree->root, output_file);

    fclose(output_file);

    return kTreeSuccess;
}

//==============================================================================

TreeErrs_t ReadTreeOutOfFile(Tree       *tree,
                             const char *file_name)
{
    CHECK(tree);

    Text tree_text = {0};

    if (ReadTextFromFile(&tree_text, file_name) != kSuccess)
    {
        printf("\nReadTreeOutOfFile() failed to read text from file\n");

        return kFailedToReadText;

        TextDtor(&tree_text);
    }

    size_t iterator = 0;

    if (CreateNodeFromInfixText(tree, nullptr, &tree->root, &tree_text, &iterator) != kTreeSuccess)
    {
        printf("ReatTreeOutOfFile() failed to read tree");

        return kFailedToReadTree;
    }

    GRAPH_DUMP_TREE(tree);

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t CreateNodeFromInfixText(Tree      *tree,
                                          TreeNode  *parent_node,
                                          TreeNode **curr_node,
                                          Text      *text,
                                          size_t    *iterator)
{
    CHECK(tree);
    CHECK(curr_node);
    CHECK(iterator);
    CHECK(text);

    TreeErrs_t status = kTreeSuccess;


    if (*text->lines_ptr[*iterator] == '(')
    {
        status = s_NodeCtor(tree, parent_node, curr_node, (char *) kPreCtored);

        if (status != kTreeSuccess)
        {
            return status;
        }

        ++(*iterator);
    }

    status = CreateNodeFromInfixBrackets(tree, *curr_node, &(*curr_node)->left, text, iterator);

    if (status != kTreeSuccess)
    {
        return status;
    }

    SetUpData(*curr_node, text->lines_ptr[*iterator]);

    ++(*iterator);

    status = CreateNodeFromInfixBrackets(tree, *curr_node, &(*curr_node)->right, text, iterator);

    if (status != kTreeSuccess)
    {
        return status;
    }

    if (*text->lines_ptr[*iterator] == ')')
    {
        return kTreeSuccess;
    }

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t CreateNodeFromText(Tree      *tree,
                                     TreeNode  *parent_node,
                                     TreeNode **curr_node,
                                     Text      *text,
                                     size_t    *iterator)
{
    CHECK(tree);
    CHECK(curr_node);
    CHECK(iterator);
    CHECK(text);

    TreeErrs_t status = kTreeSuccess;

    if (*text->lines_ptr[*iterator] == '(')
    {
        ++(*iterator);

        status = s_NodeCtor(tree, parent_node, curr_node, text->lines_ptr[*iterator]);

        if (status != kTreeSuccess)
        {
            return status;
        }

        ++(*iterator);
    }

    status = CreateNodeFromBrackets(tree, *curr_node, &(*curr_node)->left, text, iterator);

    if (status != kTreeSuccess)
    {
        return status;
    }

    status = CreateNodeFromBrackets(tree, *curr_node, &(*curr_node)->right, text, iterator);

    if (status != kTreeSuccess)
    {
        return status;
    }

    if (*text->lines_ptr[*iterator] == ')')
    {
        return kTreeSuccess;
    }

    return kTreeSuccess;
}

//==============================================================================

static TreeErrs_t CreateNodeFromBrackets(Tree      *tree,
                                         TreeNode  *parent_node,
                                         TreeNode **node,
                                         Text      *text,
                                         size_t    *iterator)
{
    if (*text->lines_ptr[*iterator] == '(')
    {
        TreeErrs_t status = CreateNodeFromText(tree, parent_node, node, text, iterator);

        if (status != kTreeSuccess)
        {
            return status;
        }

        ++(*iterator);
    }
    else if (*text->lines_ptr[*iterator] != ')')
    {
        if (strcmp(text->lines_ptr[*iterator], "null") == 0)
        {
            *node = nullptr;
        }
        else
        {
            TreeErrs_t status = s_NodeCtor(tree, parent_node, node, text->lines_ptr[*iterator]);

            if (status != kTreeSuccess)
            {
                return status;
            }
        }

        ++(*iterator);
    }

    return kTreeSuccess;
}
//==============================================================================

static TreeErrs_t CreateNodeFromInfixBrackets(Tree      *tree,
                                              TreeNode  *parent_node,
                                              TreeNode **node,
                                              Text      *text,
                                              size_t    *iterator)
{
    if (*text->lines_ptr[*iterator] == '(')
    {
        TreeErrs_t status = CreateNodeFromInfixText(tree, parent_node, node, text, iterator);

        if (status != kTreeSuccess)
        {
            return status;
        }

        ++(*iterator);
    }
    else if (*text->lines_ptr[*iterator] != ')')
    {
        if (strcmp(text->lines_ptr[*iterator], "null") == 0)
        {
            *node = nullptr;
        }
        else
        {
            TreeErrs_t status = s_NodeCtor(tree, parent_node, node, text->lines_ptr[*iterator]);

            if (status != kTreeSuccess)
            {
                return status;
            }
        }

        ++(*iterator);
    }

    return kTreeSuccess;
}

//==============================================================================

TreeNode *CopyNode(const TreeNode *src_node,
                   TreeNode       *parent_node)
{
    if (src_node == nullptr)
    {
        return nullptr;
    }

    TreeNode *node = (TreeNode *) calloc(1, sizeof(TreeNode));

    node->data = src_node->data;

    node->type = src_node->type;

    node->left   = CopyNode(src_node->left, node);
    node->right  = CopyNode(src_node->right, node);
    node->parent = parent_node;

    return node;
}

//==============================================================================

TreeErrs_t SetParents(TreeNode *parent_node)
{
    if (parent_node == nullptr)
    {
        return kTreeSuccess;
    }

    if (parent_node->left != nullptr)
    {
        parent_node->left->parent = parent_node;

        SetParents(parent_node->left);
    }
    if (parent_node->right != nullptr)
    {
        parent_node->right->parent = parent_node;

        SetParents(parent_node->right);
    }

    return kTreeSuccess;
}
/*TreeErrs_t SwapNodesData(TreeNode *node_lhs,
                         TreeNode *node_rhs)
{
    TreeDataType_t tmp = nullptr;

    CHECK(node_lhs);
    CHECK(node_rhs);

    tmp = node_lhs->data;

    node_lhs->data = node_rhs->data;

    node_rhs->data = tmp;

    return kTreeSuccess;
}
*/
//==============================================================================

/*TreeNode *FindNode(Stack *stk,
                   TreeNode *node,
                   TreeDataType_t key)
{
    TreeNode *curr_node = nullptr;

    SeekNode(stk, node, &curr_node, key);

    return curr_node;
}
*/
//==============================================================================

/*static void SeekNode(Stack *stk,
                     TreeNode *node,
                     TreeNode **ret_node,
                     TreeDataType_t key)
{
    CHECK(stk);
    CHECK(node);
    CHECK(ret_node);

    int pop_value = 0;

    if (strcmp(node->data, key) == 0)
    {
        *ret_node = node;

        return;
    }

    // prichesat
    if (node->left != nullptr && *ret_node == nullptr)
    {
        Push(stk, kGoLeft);

        SeekNode(stk, node->left, ret_node, key);

        if (*ret_node == nullptr)
        {
            Pop(stk, &pop_value);
        }
    }

    if (node->right != nullptr && *ret_node == nullptr)
    {
        Push(stk, kGoRight);

        SeekNode(stk, node->right, ret_node, key);

        if (*ret_node == nullptr)
        {
            Pop(stk, &pop_value);
        }
    }
}*/

//==============================================================================
