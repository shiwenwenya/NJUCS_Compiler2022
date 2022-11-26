#include "tools.h"

extern int yylineno;
AST *root;
int errorLexFlag;
int errorSyntaxFlag;
extern int yyparse(void);
extern void yyrestart(FILE *);

int main(int argc, char **argv)
{
	if (argc <= 1)
		return 1;
	FILE *f = fopen(argv[1], "r");
	if (!f)
	{
		perror(argv[1]);
		return 1;
	}
	root = NULL;
	errorLexFlag = 0;	 //词法错误标志
	errorSyntaxFlag = 0; //语法错误标志
	yylineno = 1;		 //初始化yylineno
	yyrestart(f);
	yyparse();
	if (errorLexFlag == 0 && errorSyntaxFlag == 0) //没有词法错误和语法错误
	{
		printAbstractSyntaxTree(root, 0); //构建语法树
	}
	return 0;
}
