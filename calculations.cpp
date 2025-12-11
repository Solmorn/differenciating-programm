#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


#include "calculations.h"
#include "tree_file_work.h"


#define MAX_LINE_SIZE_READING "29"
#define MAX_LINE_SIZE          30
static const double EPSILON_FOR_CALCULATIONS = 0.000001;
static const double CONST_E                  = exp(1.0);
static const double CONST_PI                 = 3.14159265358979323846;
static bool AreThereVariable(TreeNode* node);


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

    printf(GREEN "\n%.5lg\n" RESET, CalculateNode(tree, tree->root_node_ptr));

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


#include "dsl_on.h"



static TreeNode* Differenciate(Tree* tree, TreeNode* node, size_t var_ind, FILE* file) {
    TreeNode* ret_ptr = nullptr;
    if (file)  {
        fprintf(file, "\nAHAHAHAHAHHA\n"
                          "\\begin{dmath*}\n    \\frac{d}{dx}\\left(");
        MakeTexFromSubtree(tree, node, file);
        fprintf(file, "\\right) = ");
    }
    printf("r");
    switch (node->value_type) {
        case Op:
            switch (node->data.op) {
                case Sub:
                    IF_F(SUB_P(DL_P, 
                               DR_P, 0))
                    ret_ptr = SUB_(DL_, DR_); break;
                case Add:
                    IF_F(ADD_P(DL_P, 
                               DR_P, 0))
                    ret_ptr = ADD_(DL_, DR_); break;
                case Mul:
                    IF_F(ADD_P(MUL_P(DL_P, 
                                     CR_P, 0), 
                               MUL_P(CL_P, 
                                     DR_P, 0), 0))
                    ret_ptr = ADD_(MUL_(DL_, CR_), MUL_(CL_, DR_)); break;
                case Div:
                    IF_F(DIV_P(SUB_P(MUL_P(DL_P,
                                           CR_P, 0),
                                     MUL_P(CL_P, 
                                           DR_P, 0), 0),
                               POW_P(CR_P, 
                                     NUM_P(2.0))))
                    ret_ptr = DIV_(SUB_(MUL_(DL_, CR_), MUL_(CL_, DR_)), POW_(CR_, NUM_(2.0))); break;
                case Pow:
                    if (AreThereVariable(node->son1) &&  AreThereVariable(node->son2)) {
                        IF_F(MUL_P(POW_P(CL_P, 
                                         CR_P), 
                                   ADD_P(MUL_P(DIV_P(DL_P, 
                                                     CL_P), 
                                               CR_P, 0), 
                                         MUL_P(DR_P, 
                                               LN_P(CL_P), 0), 0), 0))
                        ret_ptr = MUL_(POW_(CL_, CR_), ADD_(MUL_(DIV_(DL_, CL_), CR_), MUL_(DR_, LN_(CL_))));
                    } else if (!AreThereVariable(node->son1) &&  AreThereVariable(node->son2)) {
                        IF_F(MUL_P(POW_P(CL_P, 
                                         CR_P), 
                             MUL_P(DR_P, 
                                   LN_P(CL_P), 0), 0))
                        ret_ptr = MUL_(POW_(CL_, CR_), MUL_(DR_, LN_(CL_)));
                    } else if (AreThereVariable(node->son1) &&  !AreThereVariable(node->son2)) {
                        IF_F(MUL_P(CR_P, 
                                   MUL_P(POW_P(CL_P, 
                                               SUB_P(CR_P, 
                                                     NUM_P(1.0), 0)), 
                                        DL_P, 0), 0))
                        ret_ptr = MUL_(CR_, MUL_(POW_(CL_, SUB_(CR_, NUM_(1.0))), DL_));
                    } else ret_ptr = NUM_(0.0);
                    break;
                case Ln:
                    IF_F(DIV_P(DL_P, 
                               CL_P))
                    ret_ptr = DIV_(DL_, CL_); break;
                case Sin:
                    IF_F(MUL_P(DL_P, 
                         COS_P(CL_P), 0))
                    ret_ptr = MUL_(DL_, COS_(CL_)); break;
                case Cos:
                    IF_F(SUB_P(NUM_P(0.0), 
                               MUL_P(DL_P, 
                                     SIN_P(CL_P), 0), 0))
                    ret_ptr = SUB_(NUM_(0.0), MUL_(DL_, SIN_(CL_))); break;     
                case Tg:
                    IF_F(DIV_P(DL_P, 
                               POW_P(COS_P(CL_P), 
                                     NUM_P(2.0))))
                    ret_ptr = DIV_(DL_, POW_(COS_(CL_), NUM_(2.0))); break;
                case Arcsin:
                    IF_F(DIV_P(DL_P, 
                               POW_P(SUB_P(NUM_P(1.0), 
                                           POW_P(CL_P, 
                                                 NUM_P(2.0)), 0), 
                                     NUM_P(0.5))))
                    ret_ptr = DIV_(DL_, POW_(SUB_(NUM_(1.0), POW_(CL_, NUM_(2.0))), NUM_(0.5))); break;
                case Arccos:
                    IF_F(SUB_P(NUM_P(0.0), 
                               DIV_P(DL_P, 
                                     POW_P(SUB_P(NUM_P(1.0), 
                                                 POW_P(CL_P, 
                                                       NUM_P(2.0)), 0), 
                                           NUM_P(0.5))), 0);)
                    ret_ptr = SUB_(NUM_(0.0), DIV_(DL_, POW_(SUB_(NUM_(1.0), POW_(CL_, NUM_(2.0))), NUM_(0.5)))); break;
                case Arctg:
                    IF_F(DIV_P(DL_P, 
                               ADD_P(NUM_P(1.0), 
                                     POW_P(CL_P, 
                                           NUM_P(2.0)), 0)))
                    ret_ptr = DIV_(DL_, ADD_(NUM_(1.0), POW_(CL_, NUM_(2.0)))); break;
                case Sh:
                    IF_F(MUL_P(DL_P, 
                               CH_P(CL_P), 0))
                    ret_ptr = MUL_(DL_, CH_(CL_)); break;
                case Ch:
                    ret_ptr = MUL_(DL_, SH_(CL_)); break;
                case Th:
                    IF_F(DIV_P(DL_P, 
                               POW_P(CH_P(CL_P),
                                     NUM_P(2.0))))
                    ret_ptr = DIV_(DL_, POW_(CH_(CL_), NUM_(2.0))); break;
                case NoOp:
                default:
                    break;
            }
            break;
        case Var:
            if (node->data.var_ind == var_ind) {
                IF_F(NUM_P(1.0))
                ret_ptr = NUM_(1.0);
            } else {
                IF_F(NUM_P(0.0))
                ret_ptr = NUM_(0.0);
            }
            break;
        case Num:
            IF_F(NUM_P(0.0))
            ret_ptr = NUM_(0.0); break;
        case NoType:
        default:
            break;
    }
    if (file) AddPhraseAndDirevativeToTexFile(tree, node, ret_ptr, var_ind, file);
    return ret_ptr;
}

#include "dsl_off.h"


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

void MakeTreeEasy(Tree* tree, FILE* file) {

    if (file) {
        fprintf(file, "\\subsection{Let's simplify this equation:}\n");
        AddTreeToTexFile(tree, file);
    }

    size_t old_num_of_elems = tree->number_of_elements;
    tree->root_node_ptr = MakeItEasy(tree, tree->root_node_ptr);
    while (old_num_of_elems != tree->number_of_elements) {
        old_num_of_elems = tree->number_of_elements;
        tree->root_node_ptr = MakeItEasy(tree, tree->root_node_ptr);
    }

    if (file) {
        fprintf(file, "Now it looks nice!\n");
        AddTreeToTexFile(tree, file);
    }

    //TreeDump(tree, HTMLFileMode, "Func: %s\nEasier tree\n", __func__);
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


void FillTreeWithDiff(Tree* diff_tree, Tree* tree, FILE* file) {
    CopyTreeVars(diff_tree, tree);
    
    diff_tree->root_node_ptr = Differenciate(diff_tree, tree->root_node_ptr, 0, file);
    diff_tree->root_node_ptr->parent = nullptr;

    //TreeDump(diff_tree, HTMLFileMode, "Func: %s\nDifferenciated tree\n", __func__);
}



void FillTreeWithOrederedDiff(Tree* ordered_diff_tree, Tree* tree, size_t diff_number, FILE* file) {

    if (file) {
        fprintf(file, "\\section{Let's find the beautiful %lu direvative of this equation:}\n", diff_number);
        AddTreeToTexFile(tree, file);
    }

    CopyTree(ordered_diff_tree, tree);

    if (diff_number > 0) {
        Tree temp_tree = {};
        INIT_TREE(temp_tree);

        for (size_t cur_diff_num = 0; cur_diff_num < diff_number; cur_diff_num++) {

            if (file) {
                fprintf(file, "\\subsection{Let's find  %lu direvative of the equation}\n", cur_diff_num + 1);
            }

            CopyTree(&temp_tree, ordered_diff_tree);
            KillSubTree(ordered_diff_tree, ordered_diff_tree->root_node_ptr);
            FillTreeWithDiff(ordered_diff_tree, &temp_tree, file);
            MakeTreeEasy(ordered_diff_tree, file);
            KillSubTree(&temp_tree, temp_tree.root_node_ptr);
        }
        KillTree(&temp_tree);
    }

    if (file) {
        fprintf(file, "\\section{Let's refresh our knoewledge about the function}\n");
        fprintf(file, "\\subsection{Function:}\n");
        AddTreeToTexFile(tree, file);
        fprintf(file, "\\subsection{It's %lu derivative:}\n", diff_number);
        AddTreeToTexFile(ordered_diff_tree, file);
    }

    //TreeDump(ordered_diff_tree, HTMLFileMode, "Func: %s\nDifferenciated %lu times tree\n", __func__, diff_number);
}

static void ChangeVarsToAnother(TreeNode* node, size_t var_to_change_ind, size_t new_var_ind) {
    if (node->value_type == Var && node->data.var_ind == var_to_change_ind) node->data.var_ind = new_var_ind;
    if (node->son1) ChangeVarsToAnother(node->son1, var_to_change_ind, new_var_ind);
    if (node->son2) ChangeVarsToAnother(node->son2, var_to_change_ind, new_var_ind);
}

#include "dsl_on.h"

static TreeNode* MakeZeroTailorNode(Tree* tree, size_t var_x_ind, size_t var_x_0_ind) {

    TreeNode* node = nullptr;
    
    return ADD_(NUM_(0),
                MUL_(DIV_(tree->root_node_ptr, 
                          NUM_(1)), 
                     POW_(SUB_(VAR_(var_x_ind), 
                               VAR_(var_x_0_ind)), 
                          NUM_(0))));
}

static void AddNextTailorPart(Tree* tree, size_t tailor_number, size_t var_x_ind, size_t var_x_0_ind, FILE* file = nullptr);

static void AddNextTailorPart(Tree* tree, size_t tailor_number, size_t var_x_ind, size_t var_x_0_ind, FILE* file) {
    TreeNode* node = nullptr;
    TreeNode* diff_part = tree->root_node_ptr->son2->son1->son1;
    double    divisor   = tree->root_node_ptr->son2->son1->son2->data.num;
    double    power     = tree->root_node_ptr->son2->son2->son2->data.num;
    MakeItEasy(tree, diff_part);
    TreeNode* new_root = ADD_(tree->root_node_ptr, 
                              MUL_(DIV_(Differenciate(tree, diff_part, var_x_0_ind, file), 
                                        NUM_(divisor * (double)tailor_number)), 
                                   POW_(SUB_(VAR_(var_x_ind), 
                                             VAR_(var_x_0_ind)), 
                                        NUM_(power + 1))));
    tree->root_node_ptr = new_root;
}

#include "dsl_off.h"

void MakeTaylor(Tree* taylor_tree, Tree* tree, size_t tailor_number, size_t var_x_ind, FILE* file) {

    if (file) {
        fprintf(file, "\\section{Let's calculate first %lu parts of taylor's form of this equation:}\n", tailor_number);
        AddTreeToTexFile(tree, file);
    }

    CopyTree(taylor_tree, tree);

    size_t var_x_0_ind = taylor_tree->number_of_variables;
    taylor_tree->number_of_variables++;
    taylor_tree->vars[var_x_0_ind] = {(char*) "x_0", 3, 0};

    ChangeVarsToAnother(taylor_tree->root_node_ptr, var_x_ind, var_x_0_ind);

    taylor_tree->root_node_ptr = MakeZeroTailorNode(taylor_tree, var_x_ind, var_x_0_ind);

    for (size_t counter = 1; counter <= tailor_number; counter++) {
        AddNextTailorPart(taylor_tree, counter, var_x_ind, var_x_0_ind);
    }

    MakeTreeEasy(taylor_tree);

    if (file) {
        fprintf(file, "\\Here it is:\n");
        AddTreeToTexFile(taylor_tree, file);
    }

    //TreeDump(taylor_tree, HTMLFileMode, "Tailor done. To %lu summant", tailor_number);

}

static void SubstituteParamNode(Tree* tree, TreeNode* node, size_t subst_param_ind) {
    if (node->value_type == Var && node->data.var_ind == subst_param_ind) {
        node->value_type = Num;
        node->data = {.num = tree->vars[subst_param_ind].value};
    }

    if (node->son1) SubstituteParamNode(tree, node->son1, subst_param_ind);
    if (node->son2) SubstituteParamNode(tree, node->son2, subst_param_ind);
}

static void SubstituteParamTree(Tree* tree, size_t subst_param_ind) {
    SubstituteParamNode(tree, tree->root_node_ptr, subst_param_ind);
    tree->vars[subst_param_ind] = {nullptr, 0, 0};
    tree->number_of_variables--;
    //TreeDump(tree, HTMLFileMode, "Func: %s\nParam substituted\n", __func__);
}

void TaylorResearch(Tree* tailor_tree, Tree* tree, size_t var_x_ind, 
                    FILE* file, const char* taylor_graph_params) {

    fprintf(file, "\\section{Let's research taylor's form of this equation:}\n");
    AddTreeToTexFile(tree, file);                    

    FILE* params_file = fopen(taylor_graph_params, "r");
    FILE* for_data    = fopen("temp_gnuplot_data.dat" ,"w");
    FILE* for_point   = fopen("temp_gnuplot_point.dat", "w");
    FILE* for_gnuplot = fopen("temp_gnuplot_code.gnuplot" ,"w");
    size_t tailor_number = 0;
    double x_0_val = 0;
    double xmin    = 0;
    double xmax    = 0;
    double step    = 1;

    fscanf(params_file, "Taylor last summant's power: %lu\n", &tailor_number);
    fscanf(params_file, "Taylor starting point: %lf\n", &x_0_val);
    fscanf(params_file, "xmin: %lf\n", &xmin);
    fscanf(params_file, "xmax: %lf\n", &xmax);
    fscanf(params_file, "step: %lf\n", &step);
    fclose(params_file);

    MakeTaylor(tailor_tree, tree, tailor_number, var_x_ind);

    Tree temp_coefficient_taylor = {};
    INIT_TREE(temp_coefficient_taylor);

    CopyTree(&temp_coefficient_taylor, tailor_tree);
    temp_coefficient_taylor.vars[temp_coefficient_taylor.number_of_variables - 1].value = x_0_val;

    SubstituteParamTree(&temp_coefficient_taylor, temp_coefficient_taylor.number_of_variables - 1);
    
    MakeTreeEasy(&temp_coefficient_taylor);

    fprintf(file, "\\subsection{This is taylor's form with first %lu summants around point %.5lg}\n", tailor_number, x_0_val);
    AddTreeToTexFile(&temp_coefficient_taylor, file); 


    Tree temp_first_derivative = {};
    INIT_TREE(temp_first_derivative);

    FillTreeWithDiff(&temp_first_derivative, tree);
    MakeTreeEasy(&temp_first_derivative);

    tree->vars[tree->number_of_variables - 1].value = x_0_val;
    temp_first_derivative.vars[temp_first_derivative.number_of_variables - 1].value = x_0_val;
    double y_0_tangent = CalculateNode(tree, tree->root_node_ptr);
    double   k_tangent = CalculateNode(&temp_first_derivative, temp_first_derivative.root_node_ptr);
    fprintf(for_point, "%lf %lf\n", x_0_val, y_0_tangent);
    fclose(for_point);

    for (double x = xmin; x <= xmax; x += step) {
        temp_coefficient_taylor.vars[temp_coefficient_taylor.number_of_variables - 1].value = x;
        tree->vars[tree->number_of_variables - 1].value = x;
        double y_taylor = CalculateNode(&temp_coefficient_taylor, temp_coefficient_taylor.root_node_ptr);
        double y_function = CalculateNode(tree, tree->root_node_ptr);
        double y_tangent = k_tangent * (x - x_0_val) + y_0_tangent;
        fprintf(for_data, "%lf %lf %lf %lf\n", x, y_taylor, y_function, y_tangent);
    }
    fclose(for_data);


    fprintf(for_gnuplot,
        "set terminal pngcairo size 600,400 enhanced font 'Times,10'\n"
        "set output 'tex_graph.png'\n"
        "set grid\n"
        "set xlabel 'x'\n"
        "set ylabel 'f(x)'\n"
        "plot 'temp_gnuplot_data.dat'  using 1:2 with lines lc rgb '#ff0000' title 'Taylor', \\\n"
        "     'temp_gnuplot_data.dat'  using 1:3 with lines lc rgb '#001aff' title 'Function', \\\n"
        "     'temp_gnuplot_data.dat'  using 1:4 with lines lc rgb '#000000' title 'Tangent', \\\n"
        "     'temp_gnuplot_point.dat' using 1:2 with points pt 7 ps 1.5 lc rgb '#eeff00' title 'Touch point',\n"
        "unset output\n");




    fclose(for_gnuplot);

    system("gnuplot temp_gnuplot_code.gnuplot");

    fprintf(file, "\\subsection{And this is a graph to compare func, it's tangent and taylor's form}\n");
    fprintf(file, "\\includegraphics[width=0.9\\textwidth,height=0.9\\textheight, keepaspectratio]{tex_graph.png}\n");
 

    fprintf(file, "\\section{Let's refresh our knoewledge about the function}\n");
    fprintf(file, "\\subsection{Function:}\n");
    AddTreeToTexFile(tree, file);
    fprintf(file, "\\subsection{It's first derivative:}\n");
    AddTreeToTexFile(&temp_first_derivative, file);

    KillTree(&temp_first_derivative);
    KillTree(&temp_coefficient_taylor);

}
