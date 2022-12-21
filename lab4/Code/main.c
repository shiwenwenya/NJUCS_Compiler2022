#include "semantic.h"
#include "intermediate.h"
#include "objectcode.h"
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
	FILE *file1 = fopen(argv[1], "r");
	if (!file1)
	{
		perror(argv[1]);
		return 1;
	}
	FILE *file2 = fopen(argv[2], "wt+");
	if (!file2)
	{
		perror(argv[2]);
		return 1;
	}
	root = NULL;
	errorLexFlag = 0;
	errorSyntaxFlag = 0;
	yylineno = 1;
	yyrestart(file1);
	yyparse();
	if (errorLexFlag == 0 && errorSyntaxFlag == 0)
	{
		Program(root);
		translateProgram(root, file2);
		generateObjectCode(file2);
	}
	fclose(file1);
	fclose(file2);
	return 0;
}
