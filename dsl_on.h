#define CL_ CopySubTree(tree, node, node->son1)
#define CR_ CopySubTree(tree, node, node->son2)

#define DL_ Differenciate(tree, node->son1, var_ind, file)
#define DR_ Differenciate(tree, node->son2, var_ind, file)

#define NUM_(number)     AlocateTreeNode(tree, node, {.num     = number},  Num)
#define VAR_(index)      AlocateTreeNode(tree, node, {.var_ind = index},   Var)

#define    ADD_(son1, son2) AlocateTreeNode(tree, node, {.op  = Add},      Op, son1, son2)
#define    SUB_(son1, son2) AlocateTreeNode(tree, node, {.op  = Sub},      Op, son1, son2)
#define    MUL_(son1, son2) AlocateTreeNode(tree, node, {.op  = Mul},      Op, son1, son2)
#define    DIV_(son1, son2) AlocateTreeNode(tree, node, {.op  = Div},      Op, son1, son2)
#define    POW_(son1, son2) AlocateTreeNode(tree, node, {.op  = Pow},      Op, son1, son2)
#define     LN_(son1)       AlocateTreeNode(tree, node, {.op  = Ln},       Op, son1, NUM_(0))
#define    SIN_(son1)       AlocateTreeNode(tree, node, {.op  = Sin},      Op, son1, NUM_(0))
#define    COS_(son1)       AlocateTreeNode(tree, node, {.op  = Cos},      Op, son1, NUM_(0))
#define     TG_(son1)       AlocateTreeNode(tree, node, {.op  = Tg},       Op, son1, NUM_(0))
#define ARCSIN_(son1)       AlocateTreeNode(tree, node, {.op  = Arcsin},   Op, son1, NUM_(0))
#define ARCCOS_(son1)       AlocateTreeNode(tree, node, {.op  = Arccos},   Op, son1, NUM_(0))
#define  ARCTG_(son1)       AlocateTreeNode(tree, node, {.op  = Arctg},    Op, son1, NUM_(0))
#define     SH_(son1)       AlocateTreeNode(tree, node, {.op  = Sh},       Op, son1, NUM_(0))
#define     CH_(son1)       AlocateTreeNode(tree, node, {.op  = Ch},       Op, son1, NUM_(0))
#define     TH_(son1)       AlocateTreeNode(tree, node, {.op  = Th},       Op, son1, NUM_(0))


#define CL_P MakeTexFromSubtree(tree, node->son1, file);
#define CR_P MakeTexFromSubtree(tree, node->son2, file);

#define DL_P fprintf(file, "\\frac{d}{dx}\\left("); MakeTexFromSubtree(tree, node->son1, file); fprintf(file, "\\right)");
#define DR_P fprintf(file, "\\frac{d}{dx}\\left("); MakeTexFromSubtree(tree, node->son2, file); fprintf(file, "\\right)");


#define NUM_P(number)     fprintf(file, " %.3g ", number);
#define VAR_P(index)      fprintf(file, " %.*s ", tree->vars[index].name);

#define    ADD_P(son1, son2, brack_need) if(brack_need) fprintf(file, "\\left("); son1 fprintf(file, " + "); son2 if(brack_need) fprintf(file, "\\right)");
#define    SUB_P(son1, son2, brack_need) if(brack_need) fprintf(file, "\\left("); son1 fprintf(file, " - "); son2 if(brack_need) fprintf(file, "\\right)");
#define    MUL_P(son1, son2, brack_need) if(brack_need) fprintf(file, "\\left("); son1 fprintf(file, " \\cdot "); son2 if(brack_need) fprintf(file, "\\right)");
#define    DIV_P(son1, son2) fprintf(file, "\\frac{"); son1 fprintf(file, "}{");         son2 fprintf(file, "}");
#define    POW_P(son1, son2) fprintf(file, "\\left("); son1 fprintf(file, "\\right)^{"); son2 fprintf(file, "}");
#define     LN_P(son1)       fprintf(file, "\\log");    son1
#define    SIN_P(son1)       fprintf(file, "\\sin");    son1
#define    COS_P(son1)       fprintf(file, "\\cos");    son1
#define     TG_P(son1)       fprintf(file, "\\tan");    son1
#define ARCSIN_P(son1)       fprintf(file, "\\arcsin"); son1
#define ARCCOS_P(son1)       fprintf(file, "\\arccos"); son1
#define  ARCTG_P(son1)       fprintf(file, "\\atan");   son1
#define     SH_P(son1)       fprintf(file, "\\sinh");   son1
#define     CH_P(son1)       fprintf(file, "\\cosh");   son1
#define     TH_P(son1)       fprintf(file, "\\tanh");   son1

#define IF_F(code) if(file) {code fprintf(file, "\n\\end{dmath*}\n");}