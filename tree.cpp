#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>

#include "tree.h"

static void AddDotNodeRanks(Tree* tree, TreeNode* cur_node, FILE* out, size_t rank);
static void AddDotArrows(TreeNode* cur_node, FILE* out, size_t from);
static void MakeDotFromTree(Tree* tree, const char* filename);


void TreeCtor(Tree* tree, BirthInfo info_got = {}) {

    assert(tree);

    tree->buffer = nullptr;
    tree->error = Ok;
    tree->number_of_elements = 0;
    tree->root_node_ptr = nullptr;


    tree->info = info_got;
    char buffer_for_filename[MAX_STRING_SIZE] = {};
    sprintf(buffer_for_filename, "%s_%s_%d_%s_dump.html", tree->info.file, tree->info.func, tree->info.line, tree->info.name);
    tree->dump_filename = (const char*)buffer_for_filename;
    tree->dump_file = fopen(tree->dump_filename, "w");
    fclose(tree->dump_file);
    tree->dump_file = fopen(tree->dump_filename, "a");


    
}

void PrintNodeInfix(Tree* tree, TreeNode* node) {

    assert(node);

    //printf("(");

    if (node->son1) PrintNodeInfix(tree, node->son1);


    if          (node->value_type == Op) {
        printf(    " \"%s\" ", ops[(node->data).op].name);
    } else if   (node->value_type == Num) {
        printf(    " \"%lf\" ", (node->data).num);
    } else {
        printf(    " \"%s\" ", tree->vars[(node->data).var_ind].name);
    }

    if (node->son2) PrintNodeInfix(tree, node->son2);

    //printf(")");
}

void KillSubTree(Tree* tree, TreeNode* node) {

    assert(node);
    assert(tree);

    if (node->son1) KillSubTree(tree, node->son1);
    if (node->son2) KillSubTree(tree, node->son2);
    
    if (node->parent && node->parent->son1 == node) node->parent->son1 = nullptr;
    if (node->parent && node->parent->son2 == node) node->parent->son2 = nullptr;

    if (node) {
        if (tree->root_node_ptr == node) tree->root_node_ptr = nullptr;
        free(node);
        tree->number_of_elements--;
    }
}

void KillTree(Tree* tree) {

    assert(tree);

    if (tree->root_node_ptr != nullptr) KillSubTree(tree, tree->root_node_ptr);

    if (tree->buffer != nullptr) free(tree->buffer);

    fclose(tree->dump_file);
}

TreeNode* AlocateTreeNode(Tree* tree, TreeNode* parent, tree_type value, ValueType val_t, TreeNode* son1, TreeNode* son2) {

    TreeNode* new_node_ptr = (TreeNode*)calloc(1, sizeof(TreeNode));

    if (tree) {
        if (!tree->number_of_elements) tree->root_node_ptr = new_node_ptr;
        tree->number_of_elements++;
    }

    new_node_ptr->data       = value;
    new_node_ptr->value_type = val_t;

    new_node_ptr->parent = parent;
    new_node_ptr->son1 = son1;
    new_node_ptr->son2 = son2;
    if (son1) son1->parent = new_node_ptr;
    if (son2) son2->parent = new_node_ptr;

    return new_node_ptr;
}


static TreeErr NodeVerify(Tree* tree, TreeNode* node, size_t* number_of_elements){
    assert(node);

    if (node != tree->root_node_ptr && node->parent == nullptr)
        return ParrentError;
    if (node != tree->root_node_ptr && node->parent->son1 != node && node->parent->son2 != node)
        return ParentSonError;

    (*number_of_elements)++; 

    TreeErr status = Ok;
    if (node->son1 == nullptr && node->son2 == nullptr) {
        return Ok;
    } else if (node->son1 != nullptr && node->son2 != nullptr) {

        status = NodeVerify(tree, node->son1, number_of_elements);
        if (status != Ok)
            return status;

        status = NodeVerify(tree, node->son2, number_of_elements);
        if (status != Ok)
            return status;

        return Ok;
    }
    
    return TwoSonsError;
}


TreeErr TreeVerify(Tree* tree) {
    
    assert(tree);

    size_t number_of_elements = 0;
    TreeErr status = Ok;
    if (tree->root_node_ptr != nullptr) status = NodeVerify(tree, tree->root_node_ptr, &number_of_elements);
    
    tree->error = status;

    if (number_of_elements != tree->number_of_elements) {
        tree->error = NumberOfElementsError;
        return NumberOfElementsError;
    }

    return status;
}



static void DumpFormatString(char* buf, size_t capacity, const char* fmt, va_list ap) {
    if (!fmt) { if (capacity) buf[0] = '\0'; return; }
    vsnprintf(buf, capacity, fmt, ap);
}

void TreeDump(Tree* tree, DumpingMode mode, const char* why_dump, ...) {

    assert(tree);
    TreeVerify(tree);

    char comment[MAX_STRING_SIZE] = {};
    va_list ap = {};
    va_start(ap, why_dump);
    DumpFormatString(comment, sizeof(comment), why_dump, ap);
    va_end(ap);

    FILE* out = stdout;

    if (mode == HTMLFileMode) {
        out = tree->dump_file;
        fprintf(out, "<pre>\n<h2>\n");
        fprintf(out, "<font color=\"red\">\n");
        fprintf(out, "=========================================================================================\n"
                 "%s\n"
                 "=========================================================================================\n",
                 comment);
        fprintf(out, "</font>\n</h2> \n");
    }


    BirthInfo info_got = tree->info;
    fprintf(out, "=====INIT_INFO=====\nFILE: %s /-----/ FUCK: %s /-----/ LINE: %d /-----/ NAME: %s\n\n",
                                info_got.file, info_got.func, info_got.line, info_got.name);

    fprintf(out, "ERROR_CODE: %d\n", tree->error);
    fprintf(out, "TreeDump(%s[%p]) {\n", info_got.name, tree);

    fprintf(out, "    number_of_elements    =    %lu\t%s\n", tree->number_of_elements,  tree->error == NumberOfElementsError ? "(BAD!)" : "");


    fprintf(out, "\n}\n\n");

    if (mode == HTMLFileMode) {

        static size_t number_of_images = 0;

        char command[MAX_STRING_SIZE*STRING_SIZE_EXPANDER] = "";
        char img_file[MAX_STRING_SIZE] = "";


        const char* temp_dot_code = "temp_dot_code.dot";

        MakeDotFromTree(tree, temp_dot_code);

        sprintf(img_file, "images/img%lu.svg", number_of_images);

        sprintf(command, "dot -Tsvg %s -o %s", temp_dot_code, img_file);
        system(command);

        fprintf(out, "<img src=\"%s\" style=\"max-width: 100%%; height: auto;\" />\n", img_file);

        number_of_images++;

    }

}

static void AddDotNodeRanks(Tree* tree, TreeNode* cur_node, FILE* out, size_t rank) {

    static size_t node_num = 0;

    const char* style = "";
    if (cur_node->son1 == nullptr && cur_node->son2 == nullptr) {
        style = "penwidth=3, color=yellow";
    } else if (cur_node->parent == nullptr) {
        style = "penwidth=3, color=black";
    } else {
        style = "penwidth=3, color=gray";
    }
    //fprintf(out,
    //    "    node%lu [rank=%lu, shape=record, style=\"rounded,filled\", fillcolor=%s, label=\"{ parent: %p | ptr: %p | value: \'%s\' | { left: %p | right: %p } }\", %s];\n",
    //    node_num, rank, fill_color, cur_node->parent, cur_node, cur_node->value, cur_node->son1, cur_node->son2, style);
    //node_num++;

    fprintf(out,    "node%lu [rank=%lu,\n"
                        "shape=plaintext,\n"
                        "label=<"
                        "  <table BORDER=\"1\" CELLBORDER=\"1\" CELLSPACING=\"0\" BGCOLOR=\"lightgray\">\n"
                        "    <tr><td>parent: %p</td></tr>\n"
                        "    <tr><td BGCOLOR=\"lightblue\">ptr: %p</td></tr>\n"
                        "    <tr><td BGCOLOR=\"lightyellow\">type: \"%s\"</td></tr>\n", node_num, rank, cur_node->parent, cur_node, cur_node->value_type == Num ? "NUM" : 
                                                                                                                                    cur_node->value_type == Op  ? "OP"  :
                                                                                                                                    cur_node->value_type == Var ? "VAR" : "NO_OP");    
    
    if          (cur_node->value_type == Op) {
        fprintf(out,    "    <tr><td BGCOLOR=\"lightgreen\">data: \"%s\"</td></tr>\n", ops[(cur_node->data).op].name);
    } else if   (cur_node->value_type == Num) {
        fprintf(out,    "    <tr><td BGCOLOR=\"lightgreen\">data: \"%lf\"</td></tr>\n", (cur_node->data).num);
    } else {
        fprintf(out,    "    <tr><td BGCOLOR=\"lightgreen\">data: \"%.*s\"</td></tr>\n", tree->vars[(cur_node->data).var_ind].name_length, tree->vars[(cur_node->data).var_ind].name);
    }

    fprintf(out,        "    <tr>\n"
                        "      <td>\n"
                        "        <table BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n"
                        "          <tr><td>left: %p</td><td>right: %p</td></tr>\n"
                        "        </table>\n"
                        "      </td>\n"
                        "    </tr>\n"
                        "  </table>\n"
                        ">\n"
                    "%s];\n", cur_node->son1, cur_node->son2, style);
    node_num++;

    if (cur_node->son1) AddDotNodeRanks(tree, cur_node->son1, out, rank+1);
    if (cur_node->son2) AddDotNodeRanks(tree, cur_node->son2, out, rank+1);
}

static void AddDotArrows(TreeNode* cur_node, FILE* out, size_t from) {

    static size_t node_num = 0;

    if (cur_node->parent) {
        fprintf(out, "node%lu -> node%lu [color=green, weight=2];\n", from, node_num);
    }
    size_t from_cpy = node_num;
    node_num++;

    if (cur_node->son1) AddDotArrows(cur_node->son1, out, from_cpy);
    if (cur_node->son2) AddDotArrows(cur_node->son2, out, from_cpy);

}

static void MakeDotFromTree(Tree* tree, const char* filename) {

    FILE* out = fopen(filename, "w");
    fclose(out);

    out = fopen(filename, "a");


    fprintf(out, "digraph G {\n");
    fprintf(out, "    orientation=portrait;\n");
    fprintf(out, "    rankdir=TB;\n");


    AddDotNodeRanks(tree, tree->root_node_ptr, out, 1);

    AddDotArrows(tree->root_node_ptr, out, 0);

    fprintf(out, "}\n");

    fclose(out);
}