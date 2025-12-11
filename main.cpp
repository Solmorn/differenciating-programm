#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>


#include "tree.h"
#include "tree_file_work.h"
#include "calculations.h"








int main() {

    const char* tex_filename        = "main.tex";
    const char* tree_infix          = "tree_infix.txt";
    const char* taylor_graph_params = "taylor_graph_params.txt";

    FILE* tex_file = OpenTex(tex_filename);


    Tree infix_tree = {};
    INIT_TREE(infix_tree);

    Tree diff_tree = {};
    INIT_TREE(diff_tree);

    Tree taylor_tree = {};
    INIT_TREE(taylor_tree);

    InfixParseTreeFromFile(&infix_tree, tree_infix);
    //AddTreeToTexFile(&infix_tree, tex_file);
    //CalculateTree(&infix_tree);


    FillTreeWithOrederedDiff(&diff_tree, &infix_tree, 2, tex_file);
    //CalculateTree(&diff_tree);

    TaylorResearch(&taylor_tree, &infix_tree, 0, tex_file, taylor_graph_params);

    KillTree(&diff_tree);
    KillTree(&infix_tree);
    KillTree(&taylor_tree);

    CloseTex(tex_file);

    printf("\n\nCOMMIT_ME_I_WORK!!!\n\n");

    return 0;

}