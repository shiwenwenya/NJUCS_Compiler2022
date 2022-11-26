#ifndef TOOLS_H
#define TOOLS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define bool int
#define TRUE 1
#define FALSE 0

/* AbstractSyntaxTree 抽象语法树 */
typedef struct AbstractSyntaxTree
{
    int lineNumber;                               // 行数
    int abstractSyntaxTreeNodeType;               // 节点类型
    char *name;                                   // 节点名称
    char *value;                                  // 节点值
    struct AbstractSyntaxTree *firstChildrenNode; // 第一个孩子节点
    struct AbstractSyntaxTree *nextSiblingNode;   // 下一个兄弟节点
} AST;

extern AST *root;
extern int yylineno;

/* createNewAbstractSyntaxTreeNode 创建新语法树节点 */
AST *createNewAbstractSyntaxTreeNode(char *, char *, int, int);
/* addAbstractSyntaxTreeNode 添加语法树节点 */
void addAbstractSyntaxTreeNode(AST *, AST *);
/* deleteAbstractSyntaxTree 删除语法树 */
void deleteAbstractSyntaxTree(AST *);
/* printAbstractSyntaxTree 打印抽象语法树 */
void printAbstractSyntaxTree(AST *, int);
/* printSpaces 打印空格 */
void printSpaces(int);
/* stringComparison 字符串比较 */
bool stringComparison(char *, char *);
/* printNodeMessage 打印信息 */
void printNodeMessage(char *, char *);
/* to_Oct 转换成八进制数 */
int to_Oct(char *);
/* to_Hex 转换成十六进制数 */
int to_Hex(char *);
/* to_Dec 转换成十进制数 */
int to_Dec(char *);
/* My_atoi 自定义atoi */
int My_atoi(char *);

#endif