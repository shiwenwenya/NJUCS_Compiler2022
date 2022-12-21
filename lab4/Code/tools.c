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

/* My_itoa 自定义itoa */
char *My_itoa(int num, char *str)
{
    int i = 0;   // 指示填充str
    if (num < 0) // 如果num为负数，将num变正
    {
        num = -num;
        str[i++] = '-';
    }
    // 转换
    do
    {
        str[i++] = num % 10 + 48; // 取num最低位 字符0~9的ASCII码是48~57；简单来说数字0+48=48，ASCII码对应字符'0'
        num /= 10;                // 去掉最低位
    } while (num);                // num不为0继续循环
    str[i] = '\0';
    // 确定开始调整的位置
    int j = 0;
    if (str[0] == '-') // 如果有负号，负号不用调整
    {
        j = 1; // 从第二位开始调整
        ++i;   // 由于有负号，所以交换的对称轴也要后移1位
    }
    // 对称交换
    for (; j < i / 2; j++)
    {
        // 对称交换两端的值 其实就是省下中间变量交换a+b的值：a=a+b;b=a-b;a=a-b;
        str[j] = str[j] + str[i - 1 - j];
        str[i - 1 - j] = str[j] - str[i - 1 - j];
        str[j] = str[j] - str[i - 1 - j];
    }
    return str; // 返回转换后的值
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
    symbolTableNode->var_no = -1;
    strcpy(symbolTableNode->name, name);

    return symbolTableNode;
}

/* getSymbolTableNode 查找符号表节点 */
SymbolTableNode getSymbolTableNode(char *name, int depth)
{
    SymbolTableNode currentSymbolTableNode = hashTable[hash_pjw(name)].symbolTableNode;
    SymbolTableNode returnSymbolTableNode = NULL;
    int num = 0;
    while (currentSymbolTableNode != NULL)
    {
        if (stringComparison(currentSymbolTableNode->name, name) && depth >= currentSymbolTableNode->depth)
        {
            returnSymbolTableNode = currentSymbolTableNode;
        }
        currentSymbolTableNode = currentSymbolTableNode->sameHashSymbolTableNode;
        num++;
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
    /*if (currentSymbolTableNode != NULL)
    {
        currentSymbolTableNode->sameHashSymbolTableNode = symbolTableNode;
    }*/
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
    /*HashTableNode hashTableIterater = hashTableNode;
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
    return;*/
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
    symbolTableNode->controlScopeSymbolTableNode = NULL;
    symbolTableNode->sameHashSymbolTableNode = NULL;
    symbolTableNode->var_no = -1;
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
int insertStruct(Type type, char *name, int offset, char *structName)
{
    SymbolTableNode insertSymbolTableNode = initSymbolTableNode(type, name, 1, 0);
    insertSymbolTableNode->kind = 0;
    insertSymbolTableNode->offset = offset;
    insertSymbolTableNode->structName = structName;
    insertHashTable(insertSymbolTableNode, hashTable);
    if (structTable[hash_pjw(name)].symbolTableNode == NULL)
    {
        SymbolTableNode currentSymbolTableNode = (SymbolTableNode)malloc(sizeof(struct SymbolTableNode_));
        currentSymbolTableNode->type = type;
        currentSymbolTableNode->offset = offset;
        currentSymbolTableNode->structName = structName;
        currentSymbolTableNode->name = name;
        currentSymbolTableNode->sameHashSymbolTableNode = NULL;
        structTable[hash_pjw(name)].symbolTableNode = currentSymbolTableNode;
    }
    else
    {
        SymbolTableNode head = structTable[hash_pjw(name)].symbolTableNode;
        SymbolTableNode iterator = head;
        while (iterator->sameHashSymbolTableNode != NULL)
        {
            if (strcmp(head->name, name) == 0)
            {
                return 1;
            }
        }
        SymbolTableNode currentSymbolTableNode = malloc(sizeof(struct SymbolTableNode_));
        currentSymbolTableNode->offset = offset;
        currentSymbolTableNode->structName = structName;
        currentSymbolTableNode->type = type;
        currentSymbolTableNode->sameHashSymbolTableNode = head;
        strcpy(currentSymbolTableNode->name, name);
        char *symbolStructName = (char *)malloc(strlen(name) + 1);
        strcpy(symbolStructName, name);
        currentSymbolTableNode->structName = symbolStructName;
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
    case BASIC: // 数字
    {
        return t1->u.basic == t2->u.basic;
    }
    break;
    case ARRAY: // 数组，弱相等（维度相等）
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
    case STRUCTURE: // 结构体
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
                if (s1->type->kind == ARRAY) // 数组强相等
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

/* CheckFunc 检查未定义函数 */
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

/* insertInterTableList 双向链表插入结点 */
void insertInterCodesList(InterCodes curInterCode)
{
    curInterCode->prev = interCodeListTail;
    curInterCode->next = interCodeListHead;
    interCodeListTail->next = curInterCode;
    interCodeListHead->prev = curInterCode;
    interCodeListTail = curInterCode;
}

/* createInterCodes 创建双向链表成员 */
void createInterCodes(int kind, ...)
{
    va_list args;
    va_start(args, kind);
    InterCodes newInterCode = (InterCodes)(malloc(sizeof(struct InterCodes_)));
    newInterCode->code.kind = kind;
    newInterCode->next = NULL;
    switch (kind)
    {
    case LABEL_InterCode:
    case FUNC_InterCode:
    case GOTO_InterCode:
    case RETURN_InterCode:
    case ARG_InterCode:
    case PARAM_InterCode:
    case READ_InterCode:
    case WRITE_InterCode:
    {
        newInterCode->code.u.singleOP.op = va_arg(args, Operand);
        break;
    }
    case ASSIGN_InterCode:
    case GET_ADDR_InterCode:
    case GET_CONTENT_InterCode:
    case TO_ADDR_InterCode:
    case CALL_InterCode:
    case DEC_InterCode:
    {
        newInterCode->code.u.doubleOP.left = va_arg(args, Operand);
        newInterCode->code.u.doubleOP.right = va_arg(args, Operand);
        break;
    }
    case ADD_InterCode:
    case SUB_InterCode:
    case MUL_InterCode:
    case DIV_InterCode:
    {
        newInterCode->code.u.tripleOP.result = va_arg(args, Operand);
        newInterCode->code.u.tripleOP.op1 = va_arg(args, Operand);
        newInterCode->code.u.tripleOP.op2 = va_arg(args, Operand);
        break;
    }
    case IFGOTO_InterCode:
    {
        newInterCode->code.u.ifgotoOP.op1 = va_arg(args, Operand);
        newInterCode->code.u.ifgotoOP.relop = va_arg(args, char *);
        newInterCode->code.u.ifgotoOP.op2 = va_arg(args, Operand);
        newInterCode->code.u.ifgotoOP.label = va_arg(args, Operand);
    }
    }
    insertInterCodesList(newInterCode);
    va_end(args);
}

/* createOperand 创建符号 */
Operand createOperand(int kind, int type, ...)
{
    va_list args;
    va_start(args, type);
    Operand newOperand = (Operand)(malloc(sizeof(struct Operand_)));
    newOperand->kind = kind;
    newOperand->type = type;
    switch (kind)
    {
    case VARIABLE_OP: // 变量
    {
        newOperand->var_no = varNo;
        varNo++;
        newOperand->varName = va_arg(args, char *);
    }
    break;
    case CONSTANT_OP: // 常量
    {
        newOperand->value = va_arg(args, int);
    }
    break;
    case TEMP_OP: // 临时变量
    {
        newOperand->var_no = tempNo;
        tempNo++;
    }
    break;
    case FUNCTION_OP: // 函数
    {
        newOperand->funcName = va_arg(args, char *);
    }
    break;
    case LABEL_OP: // 标号
    {
        newOperand->var_no = labelNo;
        labelNo++;
    }
    break;
    }
    va_end(args);
    return newOperand;
}

/* printOperand 打印符号 */
void printOperand(Operand op, FILE *file)
{

    switch (op->kind)
    {
    case VARIABLE_OP: // 变量
    {
        if (op->type == ADDRESS)
        {
            fprintf(file, "&");
        }
        fprintf(file, "v%d", op->var_no);
        break;
    }
    case CONSTANT_OP: // 常量
    {
        fprintf(file, "#%d", op->value);
        break;
    }
    case TEMP_OP: // 临时变量
    {
        if (op->type == ADDRESS)
        {
            fprintf(file, "*");
        }
        fprintf(file, "t%d", op->var_no);
        break;
    }
    case FUNCTION_OP: // 函数
    {
        fprintf(file, "%s", op->funcName);
        break;
    }
    case LABEL_OP: // 标号
    {
        fprintf(file, "%d", op->var_no);
        break;
    }
    }
}

/* printInterCodes 打印中间代码 */
void printInterCodes(FILE *file)
{
    InterCodes curInterCodes = interCodeListHead->next;
    while (curInterCodes != interCodeListHead)
    {
        switch (curInterCodes->code.kind)
        {
        case LABEL_InterCode:
        {
            fprintf(file, "LABEL label");
            printOperand(curInterCodes->code.u.singleOP.op, file);
            fprintf(file, " : \n");
            break;
        } // 定义标号x LABEL x :
        case FUNC_InterCode:
        {
            fprintf(file, "FUNCTION ");
            printOperand(curInterCodes->code.u.singleOP.op, file);
            fprintf(file, " : \n");
            break;
        } // 定义函数f FUNCTION f :
        case ASSIGN_InterCode:
        {
            printOperand(curInterCodes->code.u.doubleOP.left, file);
            fprintf(file, " := ");
            printOperand(curInterCodes->code.u.doubleOP.right, file);
            fprintf(file, "\n");
            break;
        } // 赋值操作 x := y
        case ADD_InterCode:
        {
            printOperand(curInterCodes->code.u.tripleOP.result, file);
            fprintf(file, " := ");
            printOperand(curInterCodes->code.u.tripleOP.op1, file);
            fprintf(file, " + ");
            printOperand(curInterCodes->code.u.tripleOP.op2, file);
            fprintf(file, "\n");
            break;
        } // 加法操作 x := y + z
        case SUB_InterCode:
        {
            printOperand(curInterCodes->code.u.tripleOP.result, file);
            fprintf(file, " := ");
            printOperand(curInterCodes->code.u.tripleOP.op1, file);
            fprintf(file, " - ");
            printOperand(curInterCodes->code.u.tripleOP.op2, file);
            fprintf(file, "\n");
            break;
        } // 减法操作 x := y - z
        case MUL_InterCode:
        {
            printOperand(curInterCodes->code.u.tripleOP.result, file);
            fprintf(file, " := ");
            printOperand(curInterCodes->code.u.tripleOP.op1, file);
            fprintf(file, " * ");
            printOperand(curInterCodes->code.u.tripleOP.op2, file);
            fprintf(file, "\n");
            break;
        } // 乘法操作 x := y * z
        case DIV_InterCode:
        {
            printOperand(curInterCodes->code.u.tripleOP.result, file);
            fprintf(file, " := ");
            printOperand(curInterCodes->code.u.tripleOP.op1, file);
            fprintf(file, " / ");
            printOperand(curInterCodes->code.u.tripleOP.op2, file);
            fprintf(file, "\n");
            break;
        } // 除法操作 x := y / z
        case GET_ADDR_InterCode:
        {
            printOperand(curInterCodes->code.u.doubleOP.left, file);
            fprintf(file, " := &");
            printOperand(curInterCodes->code.u.doubleOP.right, file);
            fprintf(file, "\n");
            break;
        } // 取y的地址赋给x x := &y
        case GET_CONTENT_InterCode:
        {
            printOperand(curInterCodes->code.u.doubleOP.left, file);
            fprintf(file, " := *");
            printOperand(curInterCodes->code.u.doubleOP.right, file);
            fprintf(file, "\n");
            break;
        } // 取以y值为地址的内存单元的内容赋给x x := *y
        case TO_ADDR_InterCode:
        {
            fprintf(file, "*");
            printOperand(curInterCodes->code.u.doubleOP.left, file);
            fprintf(file, " := ");
            printOperand(curInterCodes->code.u.doubleOP.right, file);
            fprintf(file, "\n");
            break;
        } // 取y值赋给以x值为地址的内存单元 *x := y
        case GOTO_InterCode:
        {
            fprintf(file, "GOTO label");
            printOperand(curInterCodes->code.u.singleOP.op, file);
            fprintf(file, "\n");
            break;
        } // 无条件跳转至标号x GOTO x
        case IFGOTO_InterCode:
        {
            fprintf(file, "IF ");
            printOperand(curInterCodes->code.u.ifgotoOP.op1, file);
            fprintf(file, " %s ", curInterCodes->code.u.ifgotoOP.relop);
            printOperand(curInterCodes->code.u.ifgotoOP.op2, file);
            fprintf(file, " GOTO label");
            printOperand(curInterCodes->code.u.ifgotoOP.label, file);
            fprintf(file, "\n");
            break;
        } // 如果x与y满足[relop]关系则跳转至标号z IF x [relop] y GOTO z
        case RETURN_InterCode:
        {
            fprintf(file, "RETURN ");
            printOperand(curInterCodes->code.u.singleOP.op, file);
            fprintf(file, "\n");
            break;
        } // 退出当前函数并返回x值 RETURN x
        case DEC_InterCode:
        {
            fprintf(file, "DEC ");
            printOperand(curInterCodes->code.u.doubleOP.left, file);
            fprintf(file, " %d", curInterCodes->code.u.doubleOP.right->value);
            fprintf(file, "\n");
            break;
        } // 内存空间申请，大小为4的倍数 DEC x [size]
        case ARG_InterCode:
        {
            fprintf(file, "ARG ");
            printOperand(curInterCodes->code.u.singleOP.op, file);
            fprintf(file, "\n");
            break;
        } // 传实参x ARG x
        case CALL_InterCode:
        {
            printOperand(curInterCodes->code.u.doubleOP.left, file);
            fprintf(file, " := CALL ");
            printOperand(curInterCodes->code.u.doubleOP.right, file);
            fprintf(file, "\n");
            break;
        } // 调用函数，并将其返回值赋给x x := CALL f
        case PARAM_InterCode:
        {
            fprintf(file, "PARAM ");
            printOperand(curInterCodes->code.u.singleOP.op, file);
            fprintf(file, "\n");
            break;
        } // 函数参数声明 PARAM x
        case READ_InterCode:
        {
            fprintf(file, "READ ");
            printOperand(curInterCodes->code.u.singleOP.op, file);
            fprintf(file, "\n");
            break;
        } // 从控制台读取x的值 READ x
        case WRITE_InterCode:
        {
            fprintf(file, "WRITE ");
            printOperand(curInterCodes->code.u.singleOP.op, file);
            fprintf(file, "\n");
            break;
        } // 向控制台打印x的值 WRITE x
        }
        curInterCodes = curInterCodes->next;
    }
}

/* 复制符号 */
Operand copyOperand(Operand operand)
{
    Operand newOperand = (Operand)malloc(sizeof(struct Operand_));
    newOperand->kind = operand->kind;
    newOperand->type = operand->type;
    newOperand->varName = operand->varName;
    newOperand->var_no = operand->var_no;
    newOperand->funcName = operand->funcName;
    newOperand->value = operand->value;
    newOperand->depth = operand->depth;
    return newOperand;
}

/* 判断符号取反 */
char *NegateRelop(char *relop)
{
    char *negateRelop = NULL;
    if (stringComparison(relop, ">"))
    {
        negateRelop = "<=";
    }
    else if (stringComparison(relop, "<"))
    {
        negateRelop = ">=";
    }
    else if (stringComparison(relop, ">="))
    {
        negateRelop = "<";
    }
    else if (stringComparison(relop, "<="))
    {
        negateRelop = ">";
    }
    else if (stringComparison(relop, "=="))
    {
        negateRelop = "!=";
    }
    else if (stringComparison(relop, "!="))
    {
        negateRelop = "==";
    }
    return negateRelop;
}

/* 获取类型大小 */
int getTypeSize(Type type)
{
    if (type->kind == BASIC)
    {
        return 4;
    }
    else if (type->kind == ARRAY)
    {
        int arraySize = 1;
        Type tmpType = type;
        while (tmpType != NULL)
        {
            if (tmpType->kind != ARRAY)
                break;
            arraySize *= tmpType->u.array.size;
            tmpType = tmpType->u.array.element;
        }
        arraySize *= getTypeSize(tmpType);
        return arraySize;
    }
    else if (type->kind == STRUCTURE)
    {
        int structSize = 0;
        FieldList tmpStruct = type->u.structure.structures;
        while (tmpStruct != NULL)
        {
            structSize += getTypeSize(tmpStruct->type);
            tmpStruct = tmpStruct->nextFieldList;
        }
        return structSize;
    }
}