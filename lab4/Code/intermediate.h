#include "tools.h"
#include <stdarg.h>

/* translateProgram Program翻译 */
void translateProgram(AST *root, FILE *file);
/* translateExtDefList  ExtDefList翻译 */
void translateExtDefList(AST *root);
/* translateExtDef ExtDef翻译 */
void translateExtDef(AST *root);
/* translateExtDecList ExtDecList翻译 */
void translateExtDecList(AST *root);
/* translateVarDec VarDec翻译 */
Operand translateVarDec(AST *root);
/* translateFunDec FunDec翻译 */
void translateFunDec(AST *root);
/* translateVarList VarList翻译 */
void translateVarList(AST *root);
/* translateCompSt CompSt翻译 */
void translateCompSt(AST *root);
/* translateStmtList StmtList翻译 */
void translateStmtList(AST *root);
/* translateStmt Stmt翻译 */
void translateStmt(AST *root);
/* translateDefList DefList翻译 */
void translateDefList(AST *root);
/* translateDefList Def翻译 */
void translateDef(AST *root);
/* translateDecList DecList翻译 */
void translateDecList(AST *root);
/* translateDec Dec翻译 */
void translateDec(AST *root);
/* translateExp Exp翻译 */
Operand translateExp(AST *root);
/* translateArgs Args翻译 */
void translateArgs(AST *root, FieldList field);
/* translateParamDec ParamDec翻译 */
void translateParamDec(AST *root);
/* translateCond Cond翻译 */
void translateCond(AST *root, Operand lableTure, Operand lableFalse);