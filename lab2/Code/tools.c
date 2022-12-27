#include "tools.h"
HashTableNode_ hashTable[TABLESIZE] = {NULL};
HashTableNode_ structTable[TABLESIZE] = {NULL};
HashTableNode hashTableNode = NULL;
FunctionTable functionTable = NULL;

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

/* getChild 获取子节点 */
AST *getChild(AST *root, int childnum)
{
    if (root == NULL)
        return NULL;
    if (childnum == 0)
        return root->firstChildrenNode;
    AST *p = root;
    p = p->firstChildrenNode;
    for (int i = 0; i < childnum && p != NULL; i++)
        p = p->nextSiblingNode;
    return p;
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

/* hash_pjw 散列函数 */
unsigned int hash_pjw(char *name)
{
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~TABLESIZE)
            val = (val ^ (i >> 12)) & TABLESIZE;
    }
    return val;
}

/* initHashTable 初始化符号表 */
HashTableNode initHashTable()
{
    hashTableNode = (HashTableNode)malloc(sizeof(HashTableNode_));
    hashTableNode->symbolTableNode = NULL;
    hashTableNode->nextHashTableNode = NULL;
    return hashTableNode;
}

/* initSymbolTableNode 初始化符号表节点 */
SymbolTableNode initSymbolTableNode(Type type, char *name, int isDefined, int depth)
{
    SymbolTableNode symbolTableNode = (SymbolTableNode)malloc(sizeof(struct SymbolTableNode_));
    symbolTableNode->name = malloc((strlen(name) + 1) * sizeof(char));
    symbolTableNode->type = type;
    symbolTableNode->depth = depth;
    symbolTableNode->isDefined = isDefined;
    strcpy(symbolTableNode->name, name);
    return symbolTableNode;
}

/* getSymbolTableNode 查找符号表节点 */
SymbolTableNode getSymbolTableNode(char *name, int depth)
{
    SymbolTableNode currentSymbolTableNode = hashTable[hash_pjw(name)].symbolTableNode;
    SymbolTableNode returnSymbolTableNode = NULL;
    while (currentSymbolTableNode != NULL)
    {
        if (stringComparison(currentSymbolTableNode->name, name) && depth >= currentSymbolTableNode->depth)
        {
            returnSymbolTableNode = currentSymbolTableNode;
        }
        currentSymbolTableNode = currentSymbolTableNode->sameHashSymbolTableNode;
    }
    return returnSymbolTableNode;
}

/* insertHashTable 节点插入符号表 */
void insertHashTable(SymbolTableNode symbolTableNode, HashTableNode hashTableNode)
{
    SymbolTableNode currentSymbolTableNode = hashTableNode->symbolTableNode;
    if (currentSymbolTableNode == NULL)
    {
        hashTableNode->symbolTableNode = symbolTableNode;
    }
    else
    {
        while (currentSymbolTableNode->controlScopeSymbolTableNode != NULL)
        {
            currentSymbolTableNode = currentSymbolTableNode->controlScopeSymbolTableNode;
        }
        currentSymbolTableNode->controlScopeSymbolTableNode = symbolTableNode;
    }
    currentSymbolTableNode = hashTable[hash_pjw(symbolTableNode->name)].symbolTableNode;
    symbolTableNode->sameHashSymbolTableNode = currentSymbolTableNode;
    hashTable[hash_pjw(symbolTableNode->name)].symbolTableNode = symbolTableNode;
    return;
}

/* enterInnermostHashTable 进入哈希表最内层 */
HashTableNode enterInnermostHashTable()
{
    HashTableNode ret = malloc(sizeof(struct HashTableNode_));
    ret->nextHashTableNode = NULL;
    ret->symbolTableNode = NULL;
    HashTableNode tail = hashTableNode;
    while (tail != NULL && tail->nextHashTableNode != NULL)
        tail = tail->nextHashTableNode;
    tail->nextHashTableNode = ret;
    return ret;
}

/* deleteLocalVariable 删除局部变量 */
void deleteLocalVariable()
{
    HashTableNode hashTableIterater = hashTableNode;
    HashTableNode hashTableDelete = hashTableIterater;
    while (hashTableDelete->nextHashTableNode != NULL)
    {
        hashTableIterater = hashTableDelete;
        hashTableDelete = hashTableDelete->nextHashTableNode;
    }

    if (hashTableIterater == NULL)
    {
        return;
    }

    while (hashTableDelete->symbolTableNode != NULL)
    {
        SymbolTableNode nodeDelete = hashTableDelete->symbolTableNode;
        deleteNode(nodeDelete->name, nodeDelete->depth, hashTableDelete);
    }
    hashTableIterater->nextHashTableNode = NULL;
    hashTableDelete = NULL;
    return;
}

/* 删除节点 */
void deleteNode(char *name, int depth, HashTableNode hashTableNode)
{
    SymbolTableNode nodeDelete = getSymbolTableNode(name, depth);
    SymbolTableNode hashTableDelete = (SymbolTableNode)malloc(sizeof(struct SymbolTableNode_));
    hashTableDelete->sameHashSymbolTableNode = hashTable[hash_pjw(name)].symbolTableNode;
    SymbolTableNode hashTableIterater = (SymbolTableNode)malloc(sizeof(struct SymbolTableNode_));
    hashTableIterater->controlScopeSymbolTableNode = hashTableNode->symbolTableNode;

    while (hashTableIterater->controlScopeSymbolTableNode != nodeDelete)
        hashTableIterater = hashTableIterater->controlScopeSymbolTableNode;

    while (hashTableDelete->sameHashSymbolTableNode != nodeDelete)
        hashTableDelete = hashTableDelete->sameHashSymbolTableNode;
    hashTableDelete->sameHashSymbolTableNode = nodeDelete->sameHashSymbolTableNode;
    hashTableIterater->controlScopeSymbolTableNode = nodeDelete->controlScopeSymbolTableNode;
    hashTable[hash_pjw(name)].symbolTableNode = hashTableDelete->sameHashSymbolTableNode;
    hashTableNode->symbolTableNode = hashTableIterater->controlScopeSymbolTableNode;
    return;
}

/* createNewSymbolTableNode 创建新符号表节点 */
SymbolTableNode createNewSymbolTableNode(Type type, char *name, int kind, bool isDefined, int depth)
{
    SymbolTableNode symbolTableNode = (SymbolTableNode)malloc(sizeof(SymbolTableNode_));
    symbolTableNode->type = type;
    symbolTableNode->name = name;
    symbolTableNode->kind = kind;
    symbolTableNode->isDefined = isDefined;
    symbolTableNode->depth = depth;
    return symbolTableNode;
}

/* insertFunction 插入函数节点 */
void insertFunction(char *name, int functionLineNumber)
{
    FunctionTable currentFunctionTableNode = functionTable;
    if (currentFunctionTableNode == NULL)
    {
        currentFunctionTableNode = (FunctionTable)malloc(sizeof(struct FunctionTable_));
        functionTable = currentFunctionTableNode;
    }
    else
    {
        while (currentFunctionTableNode->next != NULL)
        {
            currentFunctionTableNode = currentFunctionTableNode->next;
        }
        currentFunctionTableNode->next = (FunctionTable)malloc(sizeof(struct FunctionTable_));
        currentFunctionTableNode = currentFunctionTableNode->next;
    }
    currentFunctionTableNode->name = name;
    currentFunctionTableNode->functionLineNumber = functionLineNumber;
    currentFunctionTableNode->next = NULL;
    return;
}

/* insertStruct 插入结构体节点 */
int insertStruct(Type type, char *name)
{
    if (structTable[hash_pjw(name)].symbolTableNode == NULL)
    {
        SymbolTableNode currentSymbolTableNode = (SymbolTableNode)malloc(sizeof(struct SymbolTableNode_));
        currentSymbolTableNode->type = type;
        currentSymbolTableNode->name = name;
        currentSymbolTableNode->sameHashSymbolTableNode = NULL;
        structTable[hash_pjw(name)].symbolTableNode = currentSymbolTableNode;
    }
    else
    {
        SymbolTableNode head = structTable[hash_pjw(name)].symbolTableNode;
        SymbolTableNode iterator = head;
        SymbolTableNode currentSymbolTableNode = (SymbolTableNode)malloc(sizeof(struct SymbolTableNode_));
        currentSymbolTableNode->type = type;
        currentSymbolTableNode->sameHashSymbolTableNode = head;
        strcpy(currentSymbolTableNode->name, name);
        structTable[hash_pjw(name)].symbolTableNode = currentSymbolTableNode;
    }
    return 0;
}

/* getStruct 查找结构体节点 */
SymbolTableNode getStruct(char *name)
{
    SymbolTableNode currentSymbolTableNode = structTable[hash_pjw(name)].symbolTableNode;
    SymbolTableNode returnSymbolTableNode = NULL;
    while (currentSymbolTableNode)

    {
        if (strcmp(currentSymbolTableNode->name, name) == 0)
            returnSymbolTableNode = currentSymbolTableNode;
        else
            currentSymbolTableNode = currentSymbolTableNode->sameHashSymbolTableNode;
    }
    return returnSymbolTableNode;
}

/* getLocalVariable 寻找局部变量 */
bool getLocalVariable(Type *type, char *name, int *isDefined, int depth, int mkd)
{
    SymbolTableNode symbolTableNode = hashTable[hash_pjw(name)].symbolTableNode;
    bool flag = FALSE;
    if (symbolTableNode == NULL)
    {
        return FALSE;
    }
    else
    {
        while (symbolTableNode != NULL)
        {
            if (stringComparison(symbolTableNode->name, name) && ((mkd == 0 && depth == symbolTableNode->depth) || (mkd == 1 && depth >= symbolTableNode->depth)))
            {
                *type = symbolTableNode->type;
                *isDefined = symbolTableNode->isDefined;
                flag = TRUE;
                break;
            }
            symbolTableNode = symbolTableNode->sameHashSymbolTableNode;
        }
    }
    return flag;
}

/* getGlobalVariable 寻找全局变量 */
bool getGlobalVariable(Type *type, char *name, int *isDefined, int depth, int *kind)
{
    SymbolTableNode symbolTableNode = hashTable[hash_pjw(name)].symbolTableNode;
    bool flag = FALSE;
    if (symbolTableNode == NULL)
    {
        return FALSE;
    }
    else
    {
        while (symbolTableNode != NULL)
        {
            if (stringComparison(symbolTableNode->name, name) && depth >= symbolTableNode->depth)
            {
                *type = symbolTableNode->type;
                *isDefined = symbolTableNode->isDefined;
                *kind = symbolTableNode->kind;
                flag = TRUE;
                break;
            }
            symbolTableNode = symbolTableNode->sameHashSymbolTableNode;
        }
    }
    return flag;
}

/* printSemanticError 打印语义错误 */
void printSemanticError(enum SemanticError errortype, int line, char *message)
{
    printf("Error type %d at Line %d: ", errortype + 1, line);
    switch (errortype)
    {
    case Undefined_Variable:
    {
        printf("Variable \"%s\" is used but haven't been defined.\n", message);
    }
    break;
    case Undefined_Function:
    {
        printf("Function \"%s\" is called but haven't been defined", message);
    }
    break;
    case Redefined_Variable_Name:
    {
        printf("Variable \"%s\" is defined more than once", message);
    }
    break;
    case Redefined_Function:
    {
        printf("Function \"%s\" is defined more than once", message);
    }
    break;
    case AssignOP_Type_Dismatch:
    {
        printf("The expression types on both sides of the assignment do not match.\n");
    }
    break;
    case Leftside_Rvalue_Error:
    {
        printf("Only a rvalue-expression appears to the left of an assignment.\n");
    }
    break;
    case Operand_Type_Dismatch:
    {
        printf("Operand types are mismatched or operand type and operator are mismatched.\n");
    }
    break;
    case Return_Type_Dismatch:
    {
        printf("Return type is mismatched.\n");
    }
    break;
    case Func_Call_Parameter_Dismatch:
    {
        printf("Parameters are mismatched when calling the function.\n");
    }
    break;
    case Operate_Others_As_Array:
    {
        printf("Operate other types of operand as array.\n");
    }
    break;
    case Operate_Basic_As_Func:
    {
        printf("Operate other types of operand as function.\n");
    }
    break;
    case Array_Float_Index:
    {
        printf("Use float as an array index.\n");
    }
    break;
    case Operate_Others_As_Struct:
    {
        printf("Operate other types of operand as structure.\n");
    }
    break;
    case Undefined_Field:
    {
        printf("Visit an unexist structure field \"%s\".\n", message);
    }

    break;
    case Redefined_Field:
    {
        printf("Field \"%s\" is defined more than once or initialized.\n", message);
    }
    break;
    case Redefined_Field_Name:
    {
        printf("The name of the structure \"%s\" duplicates the name of a defined structure or variable.\n", message);
    }
    break;
    case Undefined_Struct:
    {
        printf("Structure \"%s\" is used but haven't been defined.\n", message);
    }
    break;
    case Undefined_Function_But_Declaration:
    {
        printf("Function \"%s\" haven't been defined but declared.\n", message);
    }
    break;
    case Conflict_Decordef_Funcion:
    {
        printf("Function's statement \"%s\" conflit with defination or other statements.\n", message);
    }
    break;

    default:
    {
        printf("Other Mistakes, content is :%s\n", message);
    }
    break;
    }
}

/* typeEqual 检查两个类型是否相等 */
bool typeEqual(Type t1, Type t2)
{
    if (t1 == t2)
    {
        return TRUE;
    }

    if (t1 == NULL || t2 == NULL || t1->kind != t2->kind)
    {
        return FALSE;
    }
    switch (t1->kind)
    {
    case BASIC: //数字
    {
        return t1->u.basic == t2->u.basic;
    }
    break;
    case ARRAY: //数组，弱相等（维度相等）
    {
        Type pt1 = t1, pt2 = t2;
        int d1 = 0, d2 = 0;
        while (pt1 != NULL)
        {
            d1 += 1;
            pt1 = pt1->u.array.element;
            if (pt1->kind != ARRAY)
                break;
        }
        while (pt2 != NULL)
        {
            d2 += 1;
            pt2 = pt2->u.array.element;
            if (pt2->kind != ARRAY)
                break;
        }
        return d1 == d2;
    }
    break;
    case STRUCTURE: //结构体
    {
        FieldList s1 = t1->u.structure.structures;
        FieldList s2 = t2->u.structure.structures;
        while (s1 != NULL && s2 != NULL)
        {
            if (s1->type->kind != s2->type->kind)
            {
                return FALSE;
            }
            else
            {
                if (s1->type->kind == ARRAY) //数组强相等
                {
                    if (arrayStrongEqual(s1->type, s2->type) == FALSE)
                        return FALSE;
                }
                else
                {
                    if (typeEqual(s1->type, s2->type) == FALSE)
                    {
                        return FALSE;
                    }
                }
            }
            s1 = s1->nextFieldList;
            s2 = s2->nextFieldList;
        }
        if (s1 == NULL && s2 == NULL)
        {
            return TRUE;
        }
        return FALSE;
    }
    break;
    case FUNCTION:
    {
        if (t1->u.function.parameterNum != t2->u.function.parameterNum || typeEqual(t1->u.function.returnType, t2->u.function.returnType) == FALSE)
        {
            return FALSE;
        }
        FieldList p1 = t1->u.function.parameters;
        FieldList p2 = t2->u.function.parameters;
        for (int i = 0; i < t1->u.function.parameterNum; i++)
        {
            if (typeEqual(p1->type, p2->type) == FALSE)
                return FALSE;
            p1 = p1->nextFieldList;
            p2 = p2->nextFieldList;
        }
        return TRUE;
    }
    break;
    default:
    {
        printf("No such type!\n");
        return FALSE;
    }
    break;
    }
}

/* isExistStruct 查找结构体 */
bool isExistStruct(Type *type, char *name)
{
    if (structTable[hash_pjw(name)].symbolTableNode == NULL)
    {
        return FALSE;
    }
    SymbolTableNode symbolTableNode = structTable[hash_pjw(name)].symbolTableNode;
    while (symbolTableNode != NULL)
    {
        if (stringComparison(symbolTableNode->name, name))
        {
            *type = symbolTableNode->type;
            return TRUE;
        }
        symbolTableNode = symbolTableNode->sameHashSymbolTableNode;
        if (symbolTableNode == NULL)
        {
            break;
        }
    }
    return FALSE;
}

/* arrayStrongEqual 检查两个数组是否强相等 */
bool arrayStrongEqual(Type t1, Type t2)
{
    if (t1->u.array.size != t2->u.array.size || t1->u.array.element->kind != t2->u.array.element->kind)
    {
        return FALSE;
    }
    if (t1->u.array.element->kind == ARRAY)
    {
        return arrayStrongEqual(t1->u.array.element, t2->u.array.element);
    }
    else
    {
        return typeEqual(t1->u.array.element, t2->u.array.element);
    }
    return FALSE;
}

/*CheckFunc 检查未定义函数*/
void CheckFunc()
{
    FunctionTable functionTablenode = functionTable;
    while (functionTablenode != NULL)
    {
        SymbolTableNode funcNode = getSymbolTableNode(functionTablenode->name, 0);
        if (funcNode->isDefined == 0)
        {
            printSemanticError(Undefined_Function_But_Declaration, functionTablenode->functionLineNumber, functionTablenode->name);
        }
        functionTablenode = functionTablenode->next;
    }
}
