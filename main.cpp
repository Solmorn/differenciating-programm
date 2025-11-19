#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>


#include "tree.h"
#include "tree_file_work.h"
#include "calculations.h"








int main() {
    const char* tree_condition = "tree_easy.txt";
    //const char* tree_condition = "tree_sample.txt";

    Tree tree = {};
    INIT_TREE(tree);

    ParseTreeFromFile(&tree, tree_condition);

    KillTree(&tree);

    printf("\n\nCOMMIT_ME_I_WORK!!!\n\n");

    return 0;

}