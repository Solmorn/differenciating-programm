#ifndef CALCULATIONS_H
#define CALCULATIONS_H


#include "tree.h"

#define RESET   "\033[0m"
#define WHITE   "\033[1;30m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"




bool AreEqualDoubles(double a, double b);
TreeErr CalculateTree(Tree* tree);
void MakeTreeEasy(Tree* tree, FILE* file = nullptr);
void FillTreeWithDiff(Tree* diff_tree, Tree* tree, FILE* file = nullptr);
void FillTreeWithOrederedDiff(Tree* ordered_diff_tree, Tree* tree, size_t diff_number, FILE* file = nullptr);
void MakeTaylor(Tree* tailor_tree, Tree* tree, size_t tailor_number, size_t var_x_ind, FILE* file = nullptr);
void TaylorResearch(Tree* tailor_tree, Tree* tree, size_t var_x_ind, FILE* file, const char* taylor_graph_params);

#endif