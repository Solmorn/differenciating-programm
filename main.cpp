#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>


#include "tree.h"
#include "tree_file_work.h"
#include "calculations.h"








int main() {
    const char* tex_filename = "main.tex";
    const char* tree_condition = "tree_easy.txt";
    //const char* tree_condition = "tree_sample.txt";

    FILE* tex_file = OpenTex(tex_filename);

    Tree tree = {};
    INIT_TREE(tree);

    Tree diff_tree = {};
    INIT_TREE(diff_tree);

    ParseTreeFromFile(&tree, tree_condition);
    AddTreeToTexFile(&tree, tex_file);
    CalculateTree(&tree);

    FillTreeWithDiff(&diff_tree, &tree);
    AddTreeToTexFile(&diff_tree, tex_file);
    CalculateTree(&diff_tree);

    KillTree(&tree);
    KillTree(&diff_tree);

    CloseTex(tex_file);

    printf("\n\nCOMMIT_ME_I_WORK!!!\n\n");

    return 0;

}