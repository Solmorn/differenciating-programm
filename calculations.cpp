#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

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



TreeErr AddNewCharacter(TreeNode* node) {
    assert(node);

    printf(BLUE "\nWho did you think of? Type name:\n" RESET);

    CALLOC_ANSWER(name, MAX_LINE_SIZE);
    
    ReadLine(name);


    printf(BLUE "\nSo, What is difference between \"%s\" and \"%s\"? It:\n" RESET, name, node->data);

    CALLOC_ANSWER(new_question, MAX_LINE_SIZE); 
    ReadLine(new_question);

    while (strstr(new_question, "no") || strstr(new_question, "not") || strstr(new_question, "do not") || strstr(new_question, "does not")) {
        printf(RED "\nToo complex. Try again without negative definition:\n" RESET);
        ReadLine(new_question);
    }

    NodesInsertAtTheEnd(node, name, new_question);
    printf(MAGENTA "\nSuccess, Akinator supplemented\n" RESET);

    
    free(name);
    free(new_question);

    return Ok;
}


TreeErr NodesInsertAtTheEnd(TreeNode* node, char* name, char* question) {
    assert(node);
    assert(name);
    assert(question);

    node->son1 = AlocateTreeNode(node, name, NewMemory);
    node->son2 = AlocateTreeNode(node, node->data, NewMemory);
    if (node->data_memory_type == NewMemory) free(node->data);

    node->data = strdup(question);    
    node->data_memory_type = NewMemory;

    return Ok;
}

TreeErr Akinate(Tree* tree, TreeNode* node) {

    assert(node);

    ASSERT_OK(tree);

    if (node->son1 == nullptr && node->son2 == nullptr) {
        printf(YELLOW "\nIs it %s? (yes/no)\n" RESET, node->data);
        CALLOC_ANSWER(answer, MAX_LINE_SIZE);
        ReadLine(answer);

        if (!strncmp(answer, "yes", 3)) {
            printf(GREEN "\nSUCCESS\n" RESET);
        } else if (!strncmp(answer, "no", 2)) {
            printf(RED "\nFAIL\n" RESET);
            AddNewCharacter(node);
            TreeDump(tree, HTMLFileMode, "%s added new character \"%s\" to node[%p]", __func__, node->data, node);
            tree->number_of_elements += 2; //2 new nodes created;
        }

        printf(YELLOW "Wanna play again? (yes/no)\n" RESET);
        ReadLine(answer);

        if (!strncmp(answer, "yes", 3)) {
            free(answer);
            return Akinate(tree, tree->root_node_ptr);
        } else {
            free(answer);
            return Ok;
        }
        free(answer);

    } else if (node->son1 != nullptr && node->son2 != nullptr) {
        printf(YELLOW "\n%s? (yes/no)\n" RESET, node->data);
        CALLOC_ANSWER(answer, MAX_LINE_SIZE);
        ReadLine(answer);
        if (!strncmp(answer, "yes", 3)) {
            free(answer);
            return Akinate(tree, node->son1);
        } else if (!strncmp(answer, "no", 2)) {
            free(answer);
            return Akinate(tree, node->son2);
        }
        free(answer);
    } else {
        if      (node->son1) return Akinate(tree, node->son1);
        else if (node->son2) return Akinate(tree, node->son2);
    }

    ASSERT_OK(tree);

    return Ok;
}