#include "tools.h"

/* 创建read函数 */
void createRead();
/* 创建write函数 */
void createWrite();

/* Program 语义分析起点 */
void Program(AST *root);
/* ExtDefList ExtDefList检查 */
void ExtDefList(AST *root);
/* ExtDef ExtDef检查 */
void ExtDef(AST *root);
/* ExtDecList ExtDecList检查 */
void ExtDecList(AST *root, Type type);
/* Specifier Specifier检查 */
Type Specifier(AST *root);
/* VarDec VarDec检查 */
FieldList VarDec(AST *root, Type type);
/* FunDec FunDec检查 */
int FunDec(AST *root, bool isDefined, HashTableNode hashTableNode, Type type);
/* VarList VarList检查 */
FieldList VarList(AST *root, HashTableNode hashTableNode);
/* ParamDec ParamDec检查 */
FieldList ParamDec(AST *root);
/* CompSt CompSt检查 */
void CompSt(AST *root, HashTableNode hashTableNode, Type type);
/* StmtList StmtList检查 */
void StmtList(AST *root, HashTableNode hashTableNode, Type type);
/* Stmt Stmt检查 */
void Stmt(AST *root, HashTableNode hashTableNode, Type type);
/* DefList DefList检查 */
void DefList(AST *root, HashTableNode hashTableNode);
/* Def Def检查 */
void Def(AST *root, HashTableNode hashTableNode);
/* DecList Defcist检查 */
void DecList(AST *root, HashTableNode hashTableNode, Type type);
/* Dec Dec检查 */
void Dec(AST *root, HashTableNode hashTableNode, Type type);
/* Exp Exp检查 */
Type Exp(AST *root);
/* Args Args检查 */
int Args(AST *root, FieldList parameters);
/* StructDef StructDef检查 */
FieldList StructDef(AST *root, char *name, int curOffset, int *tmpOffset);
/* StructDec StructDef检查 */
FieldList StructDec(AST *root, Type type);