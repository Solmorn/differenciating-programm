#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


#include "calculations.h"


#define MAX_LINE_SIZE_READING "29"
#define MAX_LINE_SIZE          30



#define CALLOC_ANSWER(ptr, size)                         \
    char* ptr = (char*)calloc(size, sizeof(char));    \
    if (!ptr) {                                        \
        fprintf(stderr, "Alocation error\n");           \
        return AlocationError;                           \
    }                                                     \


static void ReadLine(char* answer) {
    assert(answer);

    scanf("%" MAX_LINE_SIZE_READING "[^\n]", answer);

    int ch = 0;
    while ((ch = getchar()) != '\n' && ch != EOF);
}


static TreeErr AskVars(Tree* tree) {

    CALLOC_ANSWER(answer, MAX_LINE_SIZE);
    for (size_t index = 0; index < tree->number_of_variables; index++) {
        printf(YELLOW "\nWhat is the value of " RESET BLUE "\"%s\"" RESET YELLOW "? Type number:\n" RESET, 
            tree->vars[index].name);
        
        int cur_str_len = 0;

        ReadLine(answer);
        sscanf(answer, "%*[^\n]%n", &cur_str_len);

        char* endptr = nullptr;
        double num = strtod(answer, &endptr);

        if (endptr == answer + cur_str_len) {
            tree->vars[index].value = num;
            
        } else {
            return VariableError;
        }
        
    }

    free(answer);

    return Ok;
}

static double CalculateNode(Tree* tree, TreeNode* node) {
    assert(tree);
    assert(node);
    
    switch (node->value_type) {
        case Op:
            switch (node->data.op) {
                case Add:
                    return CalculateNode(tree, node->son1) + CalculateNode(tree, node->son2);
                case Sub:
                    return CalculateNode(tree, node->son1) - CalculateNode(tree, node->son2);
                case Mul:
                    return CalculateNode(tree, node->son1) * CalculateNode(tree, node->son2);
                case Div:
                    return CalculateNode(tree, node->son1) / CalculateNode(tree, node->son2);
                case Pow:
                    return pow(CalculateNode(tree, node->son1), CalculateNode(tree, node->son2));
            }
            break;
        case Num:
            return node->data.num;
        case Var:
            return tree->vars[node->data.var_ind].value;
    }
}

TreeErr CalculateTree(Tree* tree) {

    AskVars(tree);

    printf(GREEN "\n%lf\n" RESET, CalculateNode(tree, tree->root_node_ptr));

    return Ok;
}


static TreeNode* CopySubTree(TreeNode* parent, TreeNode* node) {
    assert(node);
    TreeNode* new_node = AlocateTreeNode(nullptr, {.op = NoOp}, Op);
    memcpy(new_node, node, sizeof(TreeNode));
    new_node->parent = parent;

    if (node->son1) new_node->son1 = CopySubTree(new_node, node->son1);
    if (node->son2) new_node->son2 = CopySubTree(new_node, node->son2);

    return new_node;
}

static TreeNode* Differenciate(Tree* diff_tree, TreeNode* parent, TreeNode* node, size_t var_ind) {
    switch (node->value_type) {
        case Op:
            switch (node->data.op) {
                case Sub: {
                    TreeNode* new_node = AlocateTreeNode(parent, {.op = node->data.op}, Op);
                    new_node->son1 = Differenciate(diff_tree, new_node, node->son1, var_ind);
                    new_node->son2 = Differenciate(diff_tree, new_node, node->son2, var_ind);
                    return new_node;
                }
                case Add: {
                    TreeNode* new_node = AlocateTreeNode(parent, {.op = node->data.op}, Op);
                    new_node->son1 = Differenciate(diff_tree, new_node, node->son1, var_ind);
                    new_node->son2 = Differenciate(diff_tree, new_node, node->son2, var_ind);
                    return new_node;
                }
                case Mul: {
                    TreeNode* add_node = AlocateTreeNode(parent, {.op = Add}, Op);
                    TreeNode* mul_node1 = AlocateTreeNode(add_node, {.op = Mul}, Op);
                    TreeNode* mul_node2 = AlocateTreeNode(add_node, {.op = Mul}, Op);
                    add_node->son1 = mul_node1;
                    add_node->son2 = mul_node2;

                    mul_node1->son1 = Differenciate(diff_tree, mul_node1, node->son1, var_ind);
                    mul_node1->son2 = CopySubTree(mul_node1, node->son2);

                    mul_node2->son1 = CopySubTree(mul_node2, node->son1);
                    mul_node2->son2 = Differenciate(diff_tree, mul_node2, node->son2, var_ind);

                    return add_node;
                }
                case Div: {
                    TreeNode* div_node = AlocateTreeNode(parent, {.op = Div}, Op);
                    TreeNode* sub_node = AlocateTreeNode(div_node, {.op = Sub}, Op);
                    TreeNode* pow_node = AlocateTreeNode(div_node, {.op = Pow}, Op);

                    TreeNode* mul_node1 = AlocateTreeNode(sub_node, {.op = Mul}, Op);
                    TreeNode* mul_node2 = AlocateTreeNode(sub_node, {.op = Mul}, Op);

                    div_node->son1 = sub_node;
                    div_node->son2 = pow_node;

                    pow_node->son1 = CopySubTree(pow_node, node->son2);
                    pow_node->son2 = AlocateTreeNode(pow_node, {.num = 2}, Num);

                    mul_node1->son1 = Differenciate(diff_tree, mul_node1, node->son1, var_ind);
                    mul_node1->son2 = CopySubTree(mul_node1, node->son2);

                    mul_node2->son1 = CopySubTree(mul_node2, node->son1);
                    mul_node2->son2 = Differenciate(diff_tree, mul_node2, node->son2, var_ind);

                    return div_node;
                }
            }
        case Var:
            if (node->data.var_ind == var_ind) return AlocateTreeNode(parent, {.num = 1}, Num);
            else                               return AlocateTreeNode(parent, {.num = 0}, Num);
        case Num:
            return AlocateTreeNode(parent, {.num = 0}, Num);
    }
    return nullptr;
}

void FillTreeWithDiff(Tree* diff_tree, Tree* tree) {
    for (size_t index = 0; index < tree->number_of_variables; index++) {
        diff_tree->vars[index] = tree->vars[index];
    }
    diff_tree->number_of_variables = tree->number_of_variables;
    
    diff_tree->root_node_ptr = Differenciate(diff_tree, nullptr, tree->root_node_ptr, 0);

    TreeDump(diff_tree, HTMLFileMode, "Func: %s\nDifferenciated tree\n", __func__);
}