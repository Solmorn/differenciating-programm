#include "tree_file_work.h"



#define SKIP_SPACES(ptr) \
    while(*ptr == ' ') (ptr)++; \


static bool IsOperation(char* pos, size_t len, tree_type* val) {
    assert(pos);
    assert(val);

    for (size_t index = 0; index < sizeof(ops)/sizeof(ops[0]); index++) {
        if (!strncmp(pos, ops[index].name, len)) {
            (*val).op = ops[index].op;
            return true;
        }
    }

    return false;
}


static TreeErr StringToUnion(Tree* tree, char* pos, size_t len, tree_type* val, ValueType* val_t) {
    assert(pos);
    assert(val);
    assert(val_t);

    char* endptr = nullptr;

    double num = strtod(pos, &endptr);

    if (pos + len == endptr) {
        (*val).num = num;
        *val_t = Num;
    } else if (IsOperation(pos, len, val)) {
        *val_t = Op;
    } else {
        tree->vars[tree->number_of_variables].name = pos;
        tree->number_of_elements++;
    }
    return Ok;
}

static TreeNode* ParseTreeFromBuffer(Tree* tree, TreeNode* parent, char** pos) {

    assert(tree);
    assert(pos);

    SKIP_SPACES(*pos);
    if (**pos == '(') {
        (*pos)++;
        SKIP_SPACES(*pos);

        int cur_str_len = 0;
        sscanf(*pos, "\"%*[^\"]\"%n", &cur_str_len);


        tree_type val = {.op = NoOp};
        ValueType val_t = NoType;

        printf("__%d__%d_\n", cur_str_len, val_t);

        StringToUnion(tree, (*pos)+1, cur_str_len - 2, &val, &val_t); // -2 caused by " "

        TreeNode* node = AlocateTreeNode(parent, val, val_t); //offset from "
        if (tree->root_node_ptr == nullptr) tree->root_node_ptr = node;
        tree->number_of_elements++; //new node created

        *pos+=cur_str_len-1; // moved to next "
        **pos = '\0';
        (*pos)++;


        node->son1 = ParseTreeFromBuffer(tree, node, pos);
        node->son2 = ParseTreeFromBuffer(tree, node, pos);

    

        SKIP_SPACES(*pos);
        (*pos)++;

        return node;
    } else if (**pos == 'n') {
        *pos += strlen("nill");
        //TreeDump(tree, HTMLFileMode, "Parsing tree: %s\nPos:%s", __func__, *pos);
        return nullptr;
    }


    return nullptr;
    

}


TreeErr ParseTreeFromFile(Tree* tree, const char* filename) {

    assert(tree);
    assert(filename);

    struct stat buf = {};
    stat(filename, &buf);
    tree->buffer = (char*)calloc(buf.st_size + 2, sizeof(char));
    tree->buffer[buf.st_size] = '\0';
    FILE* file = fopen(filename, "r");

    fread(tree->buffer, buf.st_size, sizeof(char), file);
    fclose(file);

    char* pos = tree->buffer;
    tree->root_node_ptr = ParseTreeFromBuffer(tree, nullptr, &pos);
    TreeDump(tree, HTMLFileMode, "Func: %s\nParsed tree\n", __func__);

    FILE* test = fopen("test.txt","w");
    fprintf(test, "\n\n%s", pos);
    fclose(test);

    

    return Ok;

}


void PrintFileNodePrefix(Tree* tree, TreeNode* node, FILE* file) {

    assert(file);
    assert(node);

    fprintf(file, " ( ");

    if          (node->value_type == Op) {
        fprintf(file,    " \"%s\" ", ops[(node->data).op].name);
    } else if   (node->value_type == Num) {
        fprintf(file,    " \"%lf\" ", (node->data).num);
    } else {
        fprintf(file,    " \"%s\" ", tree->vars[(node->data).var_ind].name);
    }

    if (node->son1) PrintFileNodePrefix(tree, node->son1, file);
    else fprintf(file, " nill ");
    if (node->son2) PrintFileNodePrefix(tree, node->son2, file);
    else fprintf(file, " nill ");

    fprintf(file, " ) ");
}