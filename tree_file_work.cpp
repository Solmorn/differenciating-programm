#include "tree_file_work.h"

static const char* phrases[] = {"Obviously",
                                "As we see",
                                "Easy to understand that",
                                "Sooo",
                                "Therefore",
                                "If you think a little you will understand that",
                                "Any kid knows it",
                                "As clear as glass",
                                "Anyway",
                                "Though",
                                "Too simple",
                                "Even a cat can understand that"};

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
        bool found = false;
        for (size_t index = 0; index < tree->number_of_variables; index++) {
            if (!strncmp(tree->vars[index].name, pos, len)) {
                found = true;
                (*val).var_ind = index;
            }
        }
        if (!found) {
            tree->vars[tree->number_of_variables].name =        pos;
            tree->vars[tree->number_of_variables].name_length = (int)len;
            (*val).var_ind = tree->number_of_variables;
            tree->number_of_variables++;
        }
        *val_t = Var;
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

        StringToUnion(tree, (*pos)+1, cur_str_len - 2, &val, &val_t); // -2 caused by " "

        TreeNode* node = AlocateTreeNode(tree, parent, val, val_t); //offset from "
        if (tree->root_node_ptr == nullptr) tree->root_node_ptr = node;

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
        fprintf(file,    " \"%.5lg\" ", (node->data).num);
    } else {
        fprintf(file,    " \"%.*s\" ", tree->vars[(node->data).var_ind].name_length, tree->vars[(node->data).var_ind].name);
    }

    if (node->son1) PrintFileNodePrefix(tree, node->son1, file);
    else fprintf(file, " nill ");
    if (node->son2) PrintFileNodePrefix(tree, node->son2, file);
    else fprintf(file, " nill ");

    fprintf(file, " ) ");
}

FILE* OpenTex(const char* filename) {
    FILE* file = fopen(filename, "w");
    fprintf(file,   "\\documentclass[12pt,a4paper]{extreport}\n"
                    "\\usepackage[T2A]{fontenc}\n"
                    "\\usepackage[utf8]{inputenc}\n"
                    "\\usepackage{indentfirst}\n"
                    "\\usepackage{graphicx}\n"
                    "\\usepackage{breqn}\n"
                    "\\breqnsetup{breakdepth={1}}\n\n"
                    "\\begin{document}\n"
                    "\\tableofcontents\n"
                    "\\newpage\n\n");

    return file;
}

void CloseTex(FILE* file) {
    fprintf(file, "\n\\end{document}\n");
    fclose(file);
}

static bool NeedBrackets(TreeNode* node) {
    if (node->parent != nullptr && node->parent->value_type == Op) {
        switch (node->value_type){
            case Op: {
                Operations nd_op  = node->data.op;
                Operations par_op = node->parent->data.op;
                if (Add <= nd_op && nd_op <= Sub) if (par_op > nd_op && par_op != Div) return true;
                if (Mul <= nd_op && nd_op <=  Div) if (par_op > nd_op) return true;
                break;
            }
            case Var:
            case Num:
            case NoType:
            default:
                return false;
        }
    }
    return false;
}

void MakeTexFromSubtree(Tree* tree, TreeNode* node, FILE* file) {

    switch (node->value_type) {
        case Op:
            switch (node->data.op) {
                case Sub:
                case Add:
                case Mul:
                    if (NeedBrackets(node)) fprintf(file, "\\left(");
                    MakeTexFromSubtree(tree, node->son1, file);
                    fprintf(file, "%s", ops[node->data.op].tex_name);
                    MakeTexFromSubtree(tree, node->son2, file);
                    if (NeedBrackets(node)) fprintf(file, "\\right)");
                    break;
                case Div:
                    fprintf(file, "\\frac{");
                    MakeTexFromSubtree(tree, node->son1, file);
                    fprintf(file, "}{");
                    MakeTexFromSubtree(tree, node->son2, file);
                    fprintf(file, "}");
                    break;
                case Pow:
                    MakeTexFromSubtree(tree, node->son1, file);
                    fprintf(file, "^{");
                    MakeTexFromSubtree(tree, node->son2, file);
                    fprintf(file, "}");
                    break;
                case Ln:
                case Sin:
                case Cos:
                case Tg:
                case Arcsin:
                case Arccos:
                case Arctg:
                case Sh:
                case Ch:
                case Th:
                    fprintf(file, "%s", ops[node->data.op].tex_name);
                    if (NeedBrackets(node)) fprintf(file, "(");
                    else fprintf(file, " ");
                    MakeTexFromSubtree(tree, node->son1, file);
                    if (NeedBrackets(node)) fprintf(file, ")");
                    break;
                case NoOp:
                    break;
                default: {
                    break;
                }
            }
            break;
        case Var:
            fprintf(file, "%.*s", tree->vars[node->data.var_ind].name_length, tree->vars[node->data.var_ind].name);
            break;
        case Num:
            fprintf(file, "%.3lg", node->data.num);
            break;
        case NoType:
            break;
        default:
            break;
    }
}

void AddTreeToTexFile(Tree* tree, FILE* file) {
    fprintf(file, "\n\\begin{dmath*}\n    ");
    MakeTexFromSubtree(tree, tree->root_node_ptr, file);
    fprintf(file, "\n\\end{dmath*}\n");
}

void AddPhraseAndDirevativeToTexFile(Tree* tree, TreeNode* equation, TreeNode* direvative, size_t var_ind, FILE* file) {
    static size_t phrase_index = 0;
    phrase_index = (phrase_index + 43)*87; // just to immitate random
    phrase_index %= sizeof(phrases)/sizeof(phrases[0]);
    fprintf(file, "%s\n\n", phrases[phrase_index]);
    fprintf(file, "\n\\begin{dmath*}\n    ");
    fprintf(file, "\\frac{d}{d%.*s} \\left(", tree->vars[var_ind].name_length, tree->vars[var_ind].name);
    MakeTexFromSubtree(tree, equation, file);
    fprintf(file, "\\right) = ");
    MakeTexFromSubtree(tree, direvative, file);
    fprintf(file, "\n\\end{dmath*}\n");
}

static TreeNode* GetG(Tree* tree, char** s);
static TreeNode* GetE(Tree* tree, char** s);
static TreeNode* GetP(Tree* tree, char** s);
static TreeNode* GetPow(Tree* tree, char** s);
static TreeNode* GetT(Tree* tree, char** s);
static TreeNode* GetFuncOp(Tree* tree, char** s);
static TreeNode* GetVar(Tree* tree, char** s);
static TreeNode* GetN(Tree* tree, char** s);


TreeErr InfixParseTreeFromFile(Tree* tree, const char* filename) {

    assert(tree);
    assert(filename);

    struct stat buf = {};
    stat(filename, &buf);
    tree->buffer = (char*)calloc(buf.st_size + 2, sizeof(char));
    tree->buffer[buf.st_size] = '$';
    FILE* file = fopen(filename, "r");

    fread(tree->buffer, buf.st_size, sizeof(char), file);
    fclose(file);

    char* pos = tree->buffer;
    tree->root_node_ptr = GetG(tree, &pos);
    if (tree->root_node_ptr) TreeDump(tree, HTMLFileMode, "Func: %s\nInfix parsed tree\n", __func__);
    else                     TreeDump(tree, HTMLFileMode, "Func: %s\nSINTAX_ERROR_IN_FILE: %s\n", __func__, filename);
    FILE* test = fopen("test.txt","w");
    fprintf(test, "\n\n%s", pos);
    fclose(test);



    return Ok;

}



static TreeNode* GetG(Tree* tree, char** s) {
    TreeNode* node = GetE(tree, s);
    if (**s != '$') {
        return nullptr;
    };
    return node;
}
static TreeNode* GetE(Tree* tree, char** s) {

    TreeNode* node_1 =  GetT(tree, s);
    while (**s == '+' || **s == '-') {
        char op = **s;
        (*s)++;
        TreeNode* node_2 =  GetT(tree, s);
        if (op == '+') {
            node_1 = AlocateTreeNode(tree, nullptr, {.op = Add}, Op, node_1, node_2);
        }
        if (op == '-') {
            node_1 = AlocateTreeNode(tree, nullptr, {.op = Sub}, Op, node_1, node_2);
        }
    }
    return node_1;
}

static TreeNode* GetPow(Tree* tree, char** s) {
    TreeNode* node_1 =  GetP(tree, s);
    while (**s == '^') {
        char op = **s;
        (*s)++;
        TreeNode* node_2 = GetP(tree, s);
        if (op == '^') {
            node_1 = AlocateTreeNode(tree, nullptr, {.op = Pow}, Op, node_1, node_2);
        }
    }
    return node_1;
}

static TreeNode* GetT(Tree* tree, char** s) {
    TreeNode* node_1 =  GetPow(tree, s);
    while (**s == '*' || **s == '/') {
        char op = **s;
        (*s)++;
        TreeNode* node_2 = GetPow(tree, s);
        if (op == '*') {
            node_1 = AlocateTreeNode(tree, nullptr, {.op = Mul}, Op, node_1, node_2);
        }
        if (op == '/') {
            node_1 = AlocateTreeNode(tree, nullptr, {.op = Div}, Op, node_1, node_2);
        }
    }
    return node_1;
}

static TreeNode* GetP(Tree* tree, char** s) {
    if (**s == '(') {

        (*s)++;
        TreeNode* new_node = GetE(tree, s);

        if (**s == ')') (*s)++;
        else return nullptr;

        return new_node;
    }

    if (**s <= '9' && **s >= '0') return GetN(tree, s);

    TreeNode* node = GetFuncOp(tree, s);
    if (node) return node;

    return GetVar(tree, s);
}

static TreeNode* GetFuncOp(Tree* tree, char** s) {
    bool found = false;
    size_t index = 0;

    while (index < sizeof(ops)/sizeof(ops[0])) {
        if (!strncmp(*s, ops[index].name, strlen(ops[index].name))) {
            found = true;
            break;
        }
        index++;
    }

    if (found) {
        *s += strlen(ops[index].name);
        if (**s == '(') {
            TreeNode* node = GetP(tree, s);
            if (node == nullptr) return nullptr;
            return AlocateTreeNode(tree, nullptr, {.op = ops[index].op}, Op, node, AlocateTreeNode(tree, nullptr, {.num = 0}, Num));
        }
    }
    return nullptr;
}

static TreeNode* GetVar(Tree* tree, char** s) {
    SKIP_SPACES(*s);
    int cur_str_len = 0;
    sscanf(*s, "%*[A-Za-z0-9_]%n", &cur_str_len);

    tree_type val = {.op = NoOp};
    ValueType val_t = NoType;

    StringToUnion(tree, *s, cur_str_len, &val, &val_t);

    TreeNode* node = AlocateTreeNode(tree, nullptr, val, val_t);

    (*s) += cur_str_len;

    return node;
}

static TreeNode* GetN(Tree* tree, char** s) {
    SKIP_SPACES(*s);
    int cur_str_len = 0;
    sscanf(*s, "%*[0-9]%n", &cur_str_len);


    tree_type val = {.op = NoOp};
    ValueType val_t = NoType;

    StringToUnion(tree, *s, cur_str_len, &val, &val_t);

    TreeNode* node = AlocateTreeNode(tree, nullptr, val, val_t);

    (*s) += cur_str_len;

    return node;
}
