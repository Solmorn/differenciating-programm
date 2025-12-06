#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


#include "calculations.h"


#define MAX_LINE_SIZE_READING "29"
#define MAX_LINE_SIZE          30
static const double EPSILON_FOR_CALCULATIONS = 0.000001;
static const double CONST_E                  = exp(1.0);
static const double CONST_PI                 = 3.14159265358979323846;


#define CALLOC_ANSWER(ptr, size)                         \
    char* ptr = (char*)calloc(size, sizeof(char));    \
    if (!ptr) {                                        \
        fprintf(stderr, "Alocation error\n");           \
        return AlocationError;                           \
    }                                                     \

bool AreEqualDoubles(double a, double b) {
    if (abs(a-b) < EPSILON_FOR_CALCULATIONS) return true;
    return false;
}


static void ReadLine(char* answer) {
    assert(answer);

    scanf("%" MAX_LINE_SIZE_READING "[^\n]", answer);

    int ch = 0;
    while ((ch = getchar()) != '\n' && ch != EOF);
}


static TreeErr AskVars(Tree* tree) {

    CALLOC_ANSWER(answer, MAX_LINE_SIZE);
    for (size_t index = 0; index < tree->number_of_variables; index++) {
        printf(YELLOW "\nWhat is the value of " RESET BLUE "\"%.*s\"" RESET YELLOW "? Type number:\n" RESET, 
            tree->vars[index].name_length, tree->vars[index].name);
        
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
                case Ln:
                    return log(CalculateNode(tree, node->son1));
                case Sin:
                    return sin(CalculateNode(tree, node->son1));    
                case Cos:
                    return cos(CalculateNode(tree, node->son1));        
                case Tg:
                    return tan(CalculateNode(tree, node->son1));
                case Arcsin:
                    return asin(CalculateNode(tree, node->son1));    
                case Arccos:
                    return acos(CalculateNode(tree, node->son1));    
                case Arctg:
                    return atan(CalculateNode(tree, node->son1));
                case Sh:
                    return sinh(CalculateNode(tree, node->son1));
                case Ch:
                    return cosh(CalculateNode(tree, node->son1));
                case Th:
                    return tanh(CalculateNode(tree, node->son1));
                case NoOp:
                    return 0;
                default:
                    return 0;
            }
            break;
        case Num:
            return node->data.num;
        case Var:
            return tree->vars[node->data.var_ind].value;
        case NoType:
            return 0;
        default:
            return 0;
    }
}


TreeErr CalculateTree(Tree* tree) {

    AskVars(tree);

    printf(GREEN "\n%lf\n" RESET, CalculateNode(tree, tree->root_node_ptr));

    return Ok;
}


static TreeNode* CopySubTree(Tree* tree, TreeNode* parent, TreeNode* node) {
    assert(node);
    TreeNode* new_node = AlocateTreeNode(tree, nullptr, {.op = NoOp}, Op);
    memcpy(new_node, node, sizeof(TreeNode));
    new_node->parent = parent;

    if (node->son1) new_node->son1 = CopySubTree(tree, new_node, node->son1);
    if (node->son2) new_node->son2 = CopySubTree(tree, new_node, node->son2);

    return new_node;
}


#define CL_ CopySubTree(tree, node, node->son1)
#define CR_ CopySubTree(tree, node, node->son2)

#define DL_ Differenciate(tree, node->son1, var_ind)
#define DR_ Differenciate(tree, node->son2, var_ind)

#define NUM_(number)     AlocateTreeNode(tree, node, {.num = number},  Num)

#define    ADD_(son1, son2) AlocateTreeNode(tree, node, {.op  = Add},      Op, son1, son2)
#define    SUB_(son1, son2) AlocateTreeNode(tree, node, {.op  = Sub},      Op, son1, son2)
#define    MUL_(son1, son2) AlocateTreeNode(tree, node, {.op  = Mul},      Op, son1, son2)
#define    DIV_(son1, son2) AlocateTreeNode(tree, node, {.op  = Div},      Op, son1, son2)
#define    POW_(son1, son2) AlocateTreeNode(tree, node, {.op  = Pow},      Op, son1, son2)
#define     LN_(son1)       AlocateTreeNode(tree, node, {.op  = Ln},       Op, son1, NUM_(0))
#define    SIN_(son1)       AlocateTreeNode(tree, node, {.op  = Sin},      Op, son1, NUM_(0))
#define    COS_(son1)       AlocateTreeNode(tree, node, {.op  = Cos},      Op, son1, NUM_(0))
#define     TG_(son1)       AlocateTreeNode(tree, node, {.op  = Tg},       Op, son1, NUM_(0))
#define ARCSIN_(son1)       AlocateTreeNode(tree, node, {.op  = Arcsin},   Op, son1, NUM_(0))
#define ARCCOS_(son1)       AlocateTreeNode(tree, node, {.op  = Arccos},   Op, son1, NUM_(0))
#define  ARCTG_(son1)       AlocateTreeNode(tree, node, {.op  = Arctg},    Op, son1, NUM_(0))
#define     SH_(son1)       AlocateTreeNode(tree, node, {.op  = Sh},       Op, son1, NUM_(0))
#define     CH_(son1)       AlocateTreeNode(tree, node, {.op  = Ch},       Op, son1, NUM_(0))
#define     TH_(son1)       AlocateTreeNode(tree, node, {.op  = Th},       Op, son1, NUM_(0))




static TreeNode* Differenciate(Tree* tree, TreeNode* node, size_t var_ind) {
    switch (node->value_type) {
        case Op:
            switch (node->data.op) {
                case Sub:
                    return SUB_(DL_, DR_);
                case Add:
                    return ADD_(DL_, DR_);
                case Mul:
                    return ADD_(MUL_(DL_, CR_), MUL_(CL_, DR_));
                case Div:
                    return DIV_(SUB_(MUL_(DL_, CR_), MUL_(CL_, DR_)), POW_(CR_, NUM_(2)));
                case Pow:
                    return MUL_(POW_(CL_, CR_), ADD_(MUL_(DIV_(DL_, CL_), CR_), MUL_(DR_, LN_(CL_))));
                case Ln:
                    return DIV_(DL_, CL_);
                case Sin:
                    return MUL_(DL_, COS_(CL_));    
                case Cos:
                    return SUB_(NUM_(0), MUL_(DL_, SIN_(CL_)));        
                case Tg:
                    return DIV_(DL_, POW_(COS_(CL_), NUM_(2)));
                case Arcsin:
                    return DIV_(DL_, POW_(SUB_(NUM_(1), POW_(CL_, NUM_(2))), NUM_(0.5)));    
                case Arccos:
                    return SUB_(NUM_(0), DIV_(DL_, POW_(SUB_(NUM_(1), POW_(CL_, NUM_(2))), NUM_(0.5))));    
                case Arctg:
                    return DIV_(DL_, ADD_(NUM_(1), POW_(CL_, NUM_(2))));
                case Sh:
                    return MUL_(DL_, CH_(CL_));
                case Ch:
                    return MUL_(DL_, SH_(CL_));;
                case Th:
                    return DIV_(DL_, POW_(CH_(CL_), NUM_(2)));
                case NoOp:
                    return nullptr;
                default:
                    return nullptr;
            }
        case Var:
            if (node->data.var_ind == var_ind) return NUM_(1);
            else                               return NUM_(0);
        case Num:
            return NUM_(0);
        case NoType:
            return nullptr;
        default:
            return nullptr;
    }
    return nullptr;
}

#undef CL_
#undef CR_

#undef DL_
#undef DR_

#undef ADD_
#undef SUB_
#undef MUL_
#undef DIV_
#undef POW_
#undef NUM_

static bool AreThereVariable(TreeNode* node) {
    switch (node->value_type) {
        case Op:
            if (AreThereVariable(node->son1)) return true;
            if (AreThereVariable(node->son2)) return true;
            break;
        case Var:
            return true;
        case Num:
            break;
        case NoType:
            break;
        default:
            break;
    }
    return false;
}

static void KillSonTakeAnother(Tree* tree, TreeNode* node, TreeNode* son_to_kill, TreeNode* son_to_take) {
    if (son_to_kill) KillSubTree(tree, son_to_kill);
    TreeNode* for_free = son_to_take;
    TreeNode* parent = node->parent;
    memcpy(node, son_to_take, sizeof(TreeNode));
    node->parent = parent;
    if (node->son1) node->son1->parent = node;
    if (node->son2) node->son2->parent = node;
    free(for_free);
    tree->number_of_elements--;
}

static void TakeValueKillSons(Tree* tree, TreeNode* node, double number) {
    if (node->son1) KillSubTree(tree, node->son1);
    if (node->son2) KillSubTree(tree, node->son2);
    node->son1 = nullptr;
    node->son2 = nullptr;
    node->value_type = Num;
    node->data = {.num = number};
}

static TreeNode* MakeItEasy(Tree* tree, TreeNode* node) {

    if (AreThereVariable(node)) {
        if (node->son1) node->son1 = MakeItEasy(tree, node->son1);
        if (node->son2) node->son2 = MakeItEasy(tree, node->son2);
    } else {
        double new_num = CalculateNode(tree, node);
        TakeValueKillSons(tree, node, new_num);
    }

    if (node->value_type == Op) {
        switch (node->data.op) {
            case Sub:
                if        (node->son2->value_type == Num && AreEqualDoubles(node->son2->data.num, 0)) {
                    KillSonTakeAnother(tree, node, node->son2, node->son1);
                }
                break;
            case Add:
                if        (node->son1->value_type == Num && AreEqualDoubles(node->son1->data.num, 0)) {
                    KillSonTakeAnother(tree, node, node->son1, node->son2);
                } else if (node->son2->value_type == Num && AreEqualDoubles(node->son2->data.num, 0)) {
                    KillSonTakeAnother(tree, node, node->son2, node->son1);
                }
                break;
            case Mul:
                if        (node->son1->value_type == Num && AreEqualDoubles(node->son1->data.num, 0)) {
                    TakeValueKillSons(tree, node, 0);
                } else if (node->son2->value_type == Num && AreEqualDoubles(node->son2->data.num, 0)) {
                    TakeValueKillSons(tree, node, 0);
                } else if (node->son1->value_type == Num && AreEqualDoubles(node->son1->data.num, 1)) {
                    KillSonTakeAnother(tree, node, node->son1, node->son2);
                } else if (node->son2->value_type == Num && AreEqualDoubles(node->son2->data.num, 1)) {
                    KillSonTakeAnother(tree, node, node->son2, node->son1);
                }
                break;
            case Div:
                if        (node->son1->value_type == Num && AreEqualDoubles(node->son1->data.num, 0)) {
                    TakeValueKillSons(tree, node, 0);
                } else if (node->son2->value_type == Num && AreEqualDoubles(node->son2->data.num, 1)) {
                    KillSonTakeAnother(tree, node, node->son2, node->son1);
                }
                break;
            case Pow:
                if        (node->son2->value_type == Num && AreEqualDoubles(node->son2->data.num, 0)) {
                    TakeValueKillSons(tree, node, 1);
                } else if (node->son2->value_type == Num && AreEqualDoubles(node->son2->data.num, 1)) {
                    KillSonTakeAnother(tree, node, node->son2, node->son1);
                } else if (node->son1->value_type == Num && AreEqualDoubles(node->son1->data.num, 0)) {
                    TakeValueKillSons(tree, node, 0);
                } else if (node->son1->value_type == Num && AreEqualDoubles(node->son1->data.num, 1)) {
                    TakeValueKillSons(tree, node, 1);
                }
                break;
            case Ln:
                break;
            case Sin:
                break;  
            case Cos:
                break;        
            case Tg:
                break; 
            case Arcsin:
                break;     
            case Arccos:
                break;     
            case Arctg:
                break; 
            case Sh:
                break; 
            case Ch:
                break; 
            case Th:
                break; 
            case NoOp:
                return nullptr;
            default:
                return nullptr;
            }
    }
    return node;
}

void MakeTreeEasy(Tree* tree) {
    size_t old_num_of_elems = tree->number_of_elements;
    tree->root_node_ptr = MakeItEasy(tree, tree->root_node_ptr);
    while (old_num_of_elems != tree->number_of_elements) {
        old_num_of_elems = tree->number_of_elements;
        tree->root_node_ptr = MakeItEasy(tree, tree->root_node_ptr);
    }
    TreeDump(tree, HTMLFileMode, "Func: %s\nEasier tree\n", __func__);
}

static void CopyTreeVars(Tree* tree_to, Tree* tree_from) {
    for (size_t index = 0; index < tree_from->number_of_variables; index++) {
        tree_to->vars[index] = tree_from->vars[index];
    }
    tree_to->number_of_variables = tree_from->number_of_variables;
}

static void CopyTree(Tree* tree_to, Tree* tree_from) {
    CopyTreeVars(tree_to, tree_from);
    tree_to->root_node_ptr = CopySubTree(tree_to, nullptr, tree_from->root_node_ptr);
}


void FillTreeWithDiff(Tree* diff_tree, Tree* tree) {
    CopyTreeVars(diff_tree, tree);
    
    diff_tree->root_node_ptr = Differenciate(diff_tree, tree->root_node_ptr, 0);
    diff_tree->root_node_ptr->parent = nullptr;

    TreeDump(diff_tree, HTMLFileMode, "Func: %s\nDifferenciated tree\n", __func__);
}



void FillTreeWithOrederedDiff(Tree* ordered_diff_tree, Tree* tree, size_t diff_number) {

    CopyTree(ordered_diff_tree, tree);

    if (diff_number > 0) {
        Tree temp_tree = {};
        INIT_TREE(temp_tree);

        for (size_t cur_diff_num = 0; cur_diff_num < diff_number; cur_diff_num++) {
            CopyTree(&temp_tree, ordered_diff_tree);
            KillSubTree(ordered_diff_tree, ordered_diff_tree->root_node_ptr);
            FillTreeWithDiff(ordered_diff_tree, &temp_tree);
            MakeTreeEasy(ordered_diff_tree);
            KillSubTree(&temp_tree, temp_tree.root_node_ptr);
        }
        KillTree(&temp_tree);
    }

    TreeDump(ordered_diff_tree, HTMLFileMode, "Func: %s\nDifferenciated %lu times tree\n", __func__, diff_number);
}


static TreeNode* MakeZeroTailorNode(Tree* tailor_tree, TreeNode* node) {
    ADD(num(0), mul(div(copy(node), num(1)), pow(sub(x0, x), num(0))));
}

static void AddNextTailorPart(Tree* tailor_tree, size_t tailor_number) {
    new_root = add(tailor_tree->root, mul(div(diff(tailor_tree->root), num(prev_num*tailor_number)), 
            pow(sub(x0, x), num(prev_num*tailor_number))));
    KillSubTree(tailor_tree->root);
    tailor_tree->root = new_root; nodes?
}

void MakeTailor(Tree* tailor_tree, Tree* tree, size_t tailor_number) {
    KillSubTree(tailor_tree, tailor_tree->root_node_ptr);
    tailor_tree->root_node_ptr = MakeZeroTailorNode(tailor_tree, tree->root_node_ptr);

    for (size_t counter = 0; counter < tailor_number; counter++) {
        AddNextTailorPart(tailor_tree);
    }

}
