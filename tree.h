#ifndef TREE_H
#define TREE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>



static const size_t     MAX_STRING_SIZE      =  128;
static const size_t     STRING_SIZE_EXPANDER =    2;
static const size_t     MAX_NODES_NUMBER     =  100;
static const size_t     NUMBER_OF_VARS       =   10;


enum TreeErr {
    Ok,
    AlocationError,
    NumberOfElementsError,
    DataPtrError,
    ParrentError,
    ParentSonError,
    TwoSonsError


};

enum TreeDataMemoryType {
    NoNewMemory,
    NewMemory
};


enum DumpingMode {
    HTMLFileMode,
    StdMode
};

enum Operations {
    Add,
    Sub,
    Mul,
    Div,
    Pow,
    NoOp
};

struct Operation {
    Operations op;
    const char* name;
};

static const Operation ops[] = {{Add, "+"},
                                {Sub, "-"},
                                {Mul, "*"},
                                {Div, "/"},
                                {Pow, "^"}};

enum ValueType {
    Op,
    Var,
    Num,
    NoType
};

union tree_type {
    Operations op;
    double num;
    size_t var_ind;
};

struct AdditionalTreeNodeInfo {

};

struct BirthInfo {
    const char* file;
    const char* func;
    const char* name;
    int         line;
};

struct TreeNode {
    tree_type data = {.op = NoOp};

    ValueType value_type = NoType;

    TreeDataMemoryType data_memory_type = NoNewMemory;

    TreeNode* parent = nullptr;

    TreeNode* son1 = nullptr;
    TreeNode* son2 = nullptr;

    AdditionalTreeNodeInfo add_node_info = {};
};

struct Variable {
    char* name = nullptr;
    double value = 0;
};

struct Tree {

    size_t number_of_elements = 0;

    Variable vars[NUMBER_OF_VARS] = {};
    size_t number_of_variables = 0;

    char* buffer = nullptr;

    TreeNode* root_node_ptr = nullptr;

    BirthInfo info = {};

    TreeErr error = Ok;

    const char* dump_filename = nullptr;
    FILE* dump_file = nullptr;

};

#define INIT_TREE(tree) do {                                  \
    TreeCtor(&tree, {__FILE__, __func__, #tree, __LINE__});    \
} while(0);


#define ASSERT_OK(tree) do{              \
                                        \
    TreeErr code = Ok;                \
                                        \
    if ((code = TreeVerify(tree)) != Ok) {    \
        TreeDump(tree, StdMode, "ASSERTED");                \
        printf("ASSERTION FAILED\n\n"); \
        return code;                    \
    }                                   \
                                        \
}while(0);




TreeErr TreeVerify(Tree* tree);
void TreeCtor(Tree* tree, BirthInfo info_got);
void PrintNodeInfix(Tree* tree, TreeNode* node);
void KillSubtree(Tree* tree, TreeNode* node);
void KillTree(Tree* tree);
TreeNode* AlocateTreeNode(TreeNode* parent, tree_type value, ValueType val_t);
void TreeDump(Tree* tree, DumpingMode mode, const char* why_dump, ...);



#endif