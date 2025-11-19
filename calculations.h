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



TreeErr CalculateTree(Tree* tree);
void FillTreeWithDiff(Tree* diff_tree, Tree* tree);
#endif