#include "semantic.h"

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
	errorLexFlag = 0;
	errorSyntaxFlag = 0;
	yylineno = 1;
	yyrestart(f);
	yyparse();
	if (errorLexFlag == 0 && errorSyntaxFlag == 0)
	{
		Program(root);
	}
	return 0;
}
