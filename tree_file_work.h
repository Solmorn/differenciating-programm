#ifndef TREE_FILE_WORK_H
#define TREE_FILE_WORK_H

#include <sys/stat.h>
#include <string.h>
#include <assert.h>

#include "tree.h"

TreeErr ParseTreeFromFile(Tree* tree, const char* filename);
void PrintFileNodePrefix(Tree* tree, TreeNode* node, FILE* file);

#endif