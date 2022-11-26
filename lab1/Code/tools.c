#include "tools.h"

/* createNewAbstractSyntaxTreeNode 创建新语法树节点 */
AST *createNewAbstractSyntaxTreeNode(char *name, char *value, int type, int lineNumber)
{
	AST *newAbstractSyntaxTreeNode = (AST *)malloc(sizeof(AST));
	newAbstractSyntaxTreeNode->abstractSyntaxTreeNodeType = type;
	newAbstractSyntaxTreeNode->name = malloc((strlen(name) + 1) * sizeof(char));
	newAbstractSyntaxTreeNode->value = malloc((strlen(value) + 1) * sizeof(char));
	strcpy(newAbstractSyntaxTreeNode->name, name);
	strcpy(newAbstractSyntaxTreeNode->value, value);
	newAbstractSyntaxTreeNode->lineNumber = lineNumber;
	newAbstractSyntaxTreeNode->firstChildrenNode = NULL;
	newAbstractSyntaxTreeNode->nextSiblingNode = NULL;
	return newAbstractSyntaxTreeNode;
}

/* addAbstractSyntaxTreeNode 添加语法树节点 */
void addAbstractSyntaxTreeNode(AST *pre, AST *next)
{
	if (pre != NULL && next != NULL)
	{
		next->nextSiblingNode = pre->firstChildrenNode;
		pre->firstChildrenNode = next;
		pre->lineNumber = next->lineNumber;
	}
}

/* deleteAbstractSyntaxTree 删除语法树 */
void deleteAbstractSyntaxTree(AST *root)
{
	if (root == NULL)
		return;
	deleteAbstractSyntaxTree(root->firstChildrenNode);
	deleteAbstractSyntaxTree(root->nextSiblingNode);
	free(root);
	return;
}

/* printAbstractSyntaxTree 打印抽象语法树 */
void printAbstractSyntaxTree(AST *root, int depth)
{
	if (root == NULL)
		return;
	if (root->abstractSyntaxTreeNodeType == 0)
	{
		printSpaces(depth);
		printf("%s (%d)\n", root->name, root->lineNumber);
	}
	if (root->abstractSyntaxTreeNodeType == 1)
	{
	}
	if (root->abstractSyntaxTreeNodeType == 2)
	{
		printSpaces(depth);
		printNodeMessage(root->name, root->value);
	}
	printAbstractSyntaxTree(root->firstChildrenNode, depth + 2);
	printAbstractSyntaxTree(root->nextSiblingNode, depth);
	return;
}

/* printSpaces 打印空格 */
void printSpaces(int count)
{
	for (int i = 0; i < count; i++)
	{
		printf(" ");
	}
	return;
}

/* stringComparison 字符串比较 */
bool stringComparison(char *src, char *dst)
{
	return strcmp(src, dst) == 0;
}

/* printNodeMessage 打印信息 */
void printNodeMessage(char *name, char *value)
{
	if (stringComparison(name, "ID") || stringComparison(name, "TYPE"))
	{
		printf("%s: %s\n", name, value);
		return;
	}
	else if (stringComparison(name, "INT"))
	{
		printf("%s: %d\n", name, My_atoi(value));
		return;
	}
	else if (stringComparison(name, "FLOAT"))
	{
		printf("%s: %f\n", name, atof(value));
		return;
	}
	else if (stringComparison(name, "HEX"))
	{
		printf("%s: %d\n", name, My_atoi(value));
		return;
	}
	else
	{
		printf("%s\n", name);
		return;
	}
}

/* to_Oct 转换成八进制数 */
int to_Oct(char *str)
{
	int sum = 0;
	while (isdigit(*str) && *str != '8' && *str != '9')
	{
		sum = sum * 8 + *str - '0';
		str++;
	}
	return sum;
}

/* to_Hex 转换成十六进制数 */
int to_Hex(char *str)
{
	int sum = 0;
	while (isxdigit(*str))
	{
		if (isdigit(*str))
		{
			sum = sum * 16 + (*str - '0');
		}
		else if (islower(*str))
		{
			sum = sum * 16 + (*str - 'a' + 10);
		}
		else
		{
			sum = sum * 16 + (*str - 'A' + 10);
		}
		str++;
	}
	return sum;
}

/* to_Dec 转换成十进制数 */
int to_Dec(char *str)
{
	int sum = 0;
	while (isdigit(*str))
	{
		sum = sum * 10 + *str - '0';
		str++;
	}
	return sum;
}

/* My_atoi 自定义atoi */
int My_atoi(char *str)
{
	int tmp = 1;
	int sum = 0;
	while (isspace(*str))
	{
		str++;
	}
	if (*str == '-')
	{
		tmp = -tmp;
		str++;
	}
	if (*str == '+')
	{
		str++;
	}
	if (*str == '0')
	{
		if (*(str + 1) == 'x' || *(str + 1) == 'X')
		{
			sum = to_Hex(str + 2);
		}
		else
		{
			sum = to_Oct(str + 1);
		}
	}
	else
	{
		sum = to_Dec(str);
	}
	return sum * tmp;
}