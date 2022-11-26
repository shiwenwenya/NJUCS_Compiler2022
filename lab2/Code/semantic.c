#include "semantic.h"
int curDepth = 0;
HashTableNode Table = NULL;

/*开始语义分析*/
void Program(AST *root)
{
    /*
    Program -> ExfDefList
    */
    Table = initHashTable();
    ExtDefList(getChild(root, 0));
    CheckFunc();
}

/* ExtDefList检查 */
void ExtDefList(AST *root)
{
    /*
    ExfDefList -> ExfDef ExfDefList
    | (empty)
    */
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    if (node0 != NULL && node1 != NULL)
    {
        ExtDef(node0);
        ExtDefList(node1);
    }
    if (node0 != NULL && node1 == NULL)
    {
        ExtDef(node0);
    }
}

/* ExtDef 检查 */
void ExtDef(AST *root)
{
    /*
    ExtDef -> Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
    | Specifier FunDec SEMI
    */
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    AST *node2 = getChild(root, 2);
    Type type = NULL;
    if (node0 != NULL)
    {
        type = Specifier(node0);
    }
    if (node2 != NULL)
    {
        if (node1 != NULL && stringComparison(node1->name, "ExtDecList"))
        {
            ExtDecList(node1, type);
        }
        else
        {
            AST *tmpNode1 = node1;
            if (stringComparison(node2->name, "SEMI"))
            {
                HashTableNode hashTableNode = enterInnermostHashTable();
                FunDec(tmpNode1, 0, hashTableNode, type);
                deleteLocalVariable();
            }
            else
            {
                HashTableNode hashTableNode = enterInnermostHashTable();
                FunDec(tmpNode1, 1, hashTableNode, type);
                curDepth++;
                AST *tmpNode2 = node2;
                CompSt(tmpNode2, hashTableNode, type);
                curDepth--;
                deleteLocalVariable();
            }
        }
    }
    return;
}

/* ExtDecList 检查 */
void ExtDecList(AST *root, Type type)
{
    /*
    ExtDecList -> VarDec
    | VarDec COMMA ExtDecList
    */
    FieldList f = VarDec(getChild(root, 0), type);
    Type tmpType = (Type)malloc(sizeof(struct Type_));
    int isDefined;
    if (getLocalVariable(&tmpType, f->name, &isDefined, curDepth, 0))
    {
        printSemanticError(Redefined_Variable_Name, root->lineNumber, f->name);
    }
    SymbolTableNode symbolTableNode = createNewSymbolTableNode(f->type, f->name, 0, 1, curDepth);
    insertHashTable(symbolTableNode, Table);
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    AST *node2 = getChild(root, 2);
    if (node1 == NULL)
    {
        return;
    }
    else
    {
        if (node2 != NULL)
        {
            ExtDecList(node2, type);
        }
    }
    return;
}

/* Specifier 检查 */
Type Specifier(AST *root)
{
    /*
    Specifier -> TYPE
    | StructSpecifier
    StructSpecifier -> STRUCT OptTag LC DefList RC
    | STRUCT Tag
    OptTag -> ID
    | (empty)
    Tag -> ID
    */
    AST *node0 = getChild(root, 0);
    Type type = (Type)malloc(sizeof(struct Type_));
    if (stringComparison(node0->name, "StructSpecifier"))
    {
        type->kind = STRUCTURE;
        AST *node01 = getChild(node0, 1);
        if (stringComparison(node01->name, "OptTag")) // STRUCT OptTag LC DefList RC
        {
            AST *node010 = getChild(node01, 0);
            if (stringComparison(node010->value, "Data"))
            {
                exit(0);
            }
            if (stringComparison(node010->name, "ID")) // OptTag -> ID
            {
                char *structName = node010->value;
                Type newType = (Type)malloc(sizeof(struct Type_));
                int isDefined;
                if (getLocalVariable(&newType, structName, &isDefined, curDepth, 1) == 1)
                {
                    printSemanticError(Redefined_Field_Name, node010->lineNumber, structName);
                    return NULL;
                }
                else
                {
                    type->u.structure.name = (char *)malloc(sizeof(char) * 32);
                    strcpy(type->u.structure.name, structName);
                    AST *node03 = getChild(node0, 3);
                    if (!stringComparison(node03->name, "DefList"))
                        type->u.structure.structures = NULL;
                    else
                    {
                        AST *tmpNode = node03;
                        FieldList result = NULL;
                        FieldList currentFiled = NULL;
                        while (TRUE)
                        {
                            AST *tmpNode0 = getChild(tmpNode, 0);
                            if (tmpNode0 == NULL)
                                break;
                            FieldList tmpField = StructDef(tmpNode0, structName);
                            if (result == NULL)
                            {
                                result = tmpField;
                                currentFiled = result;
                            }
                            else
                            {
                                currentFiled->nextFieldList = tmpField;
                                currentFiled = currentFiled->nextFieldList;
                            }
                            tmpNode = getChild(tmpNode, 1);
                            if (tmpNode == NULL)
                                break;
                        }
                        type->u.structure.structures = result;
                    }
                }
                insertHashTable(createNewSymbolTableNode(type, structName, 1, 1, curDepth), Table);
            }
        }
        else if (stringComparison(node01->name, "Tag"))
        {
            AST *node010 = getChild(node01, 0);
            char *nodeName = node010->value;
            Type tmpTagType = NULL;
            int tmpDefined;
            if (getLocalVariable(&tmpTagType, nodeName, &tmpDefined, curDepth, 1) == 0)
            {
                printSemanticError(Undefined_Struct, node010->lineNumber, nodeName);
                return NULL;
            }
            else if (tmpTagType == NULL || tmpTagType->kind != STRUCTURE)
            {
                printSemanticError(Undefined_Struct, node010->lineNumber, nodeName);
                return NULL;
            }
            else
                return tmpTagType;
        }
        else if (stringComparison(node01->name, "LC")) // OptTag -> (empty)
        {
            char *structName = (char *)malloc(32 + 1);
            type->u.structure.name = (char *)malloc(sizeof(char) * 32);
            strcpy(type->u.structure.name, structName);
            AST *node02 = getChild(node0, 2);
            if (!stringComparison(node02->name, "DefList"))
                type->u.structure.structures = NULL;
            else
            {
                AST *tmpNode = node02;
                FieldList result = NULL;
                FieldList currentFiled = NULL;
                while (TRUE)
                {
                    AST *tmpNode0 = getChild(tmpNode, 0);
                    if (tmpNode0 == NULL)
                        break;
                    FieldList tmpField = StructDef(tmpNode0, structName);
                    if (result == NULL)
                    {
                        result = tmpField;
                        currentFiled = result;
                    }
                    else
                    {
                        currentFiled->nextFieldList = tmpField;
                        currentFiled = currentFiled->nextFieldList;
                    }
                    tmpNode = getChild(tmpNode, 1);
                    if (tmpNode == NULL)
                        break;
                }
                type->u.structure.structures = result;
            }
        }
    }
    else if (stringComparison(node0->name, "TYPE")) // StructSpecifier
    {
        type->kind = BASIC;
        if (stringComparison(node0->value, "int"))
            type->u.basic = 0;
        else if (stringComparison(node0->value, "float"))
            type->u.basic = 1;
    }
    return type;
}

/* VarDec 检查 */
FieldList VarDec(AST *root, Type type)
{
    /*
    VarDec -> ID
    | VarDec LB INT RB
    */
    FieldList field = (FieldList)(malloc(sizeof(FieldList_)));
    field->nextFieldList = NULL;
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    if (node0 != NULL)
    {
        if (stringComparison(node0->name, "ID")) // VarDec -> ID
        {
            field->type = type;
            field->name = node0->value;
        }
        else if (stringComparison(node0->name, "VarDec")) // VarDec -> VarDec LB INT RB
        {
            FieldList tmpField = VarDec(node0, type);
            int layers = 0;
            AST *p = node0;
            while (p != NULL && p->firstChildrenNode != NULL)
            {
                p = p->firstChildrenNode;
                layers++;
            }
            field->name = tmpField->name;
            Type_ **typeList = (Type_ **)malloc(sizeof(struct Type_ **) * (layers + 2));
            layers--;
            int top = layers;
            while (node0 != NULL && node0->firstChildrenNode != NULL)
            {
                Type tmpType = (Type)malloc(sizeof(struct Type_));
                AST *tmpNode = node0->nextSiblingNode->nextSiblingNode;
                tmpType->kind = ARRAY;
                tmpType->u.array.size = My_atoi(tmpNode->value);
                typeList[layers] = tmpType;
                layers--;
                node0 = node0->firstChildrenNode;
            }
            typeList[top]->u.array.element = type;
            Type tmpType1 = typeList[0];
            Type tmpType2 = tmpType1;
            for (int i = 1; i <= top; i++)
            {
                tmpType2->u.array.element = typeList[i];
                tmpType2 = tmpType2->u.array.element;
            }
            field->type = tmpType1;
        }
    }
    return field;
}

/* FunDec 检查 */
int FunDec(AST *root, bool isDefined, HashTableNode hashTableNode, Type type)
{
    /*
    FunDec -> ID LP VarList RP
    | ID LP RP
    */
    AST *node0 = getChild(root, 0);
    AST *node2 = getChild(root, 2);
    SymbolTableNode symbolTableNode = getSymbolTableNode(node0->value, curDepth);
    int isSymbolTableNode = 0, isSymbolTableNodeDefined = 0;
    Type symbolTableNodeType = NULL;
    if (symbolTableNode != NULL)
    {
        isSymbolTableNode = 1;
        isSymbolTableNodeDefined = symbolTableNode->isDefined;
        symbolTableNodeType = symbolTableNode->type;
    }
    Type newFuncType = (Type)(malloc(sizeof(struct Type_)));
    if (stringComparison(node2->name, "VarList")) // FunDec -> ID LP VarList RP
    {
        curDepth++;
        FieldList field = VarList(node2, hashTableNode);
        curDepth--;
        newFuncType->u.function.parameters = field;
        newFuncType->u.function.parameterNum = 0;
        while (field != NULL)
        {
            newFuncType->u.function.parameterNum++;
            field = field->nextFieldList;
        }
    }
    else // FunDec -> ID LP RP
    {
        newFuncType->u.function.parameterNum = 0;
        newFuncType->u.function.parameters = NULL;
    }
    newFuncType->kind = FUNCTION;
    newFuncType->u.function.returnType = type;
    if (isSymbolTableNode == 1)
    {
        if (isDefined == 1)
        {
            if (isSymbolTableNodeDefined == 1)
            {
                printSemanticError(Redefined_Function, root->lineNumber, node0->value);
            }
            else if (typeEqual(symbolTableNodeType, newFuncType) == FALSE)
            {
                printSemanticError(Conflict_Decordef_Funcion, root->lineNumber, node0->value);
            }
            else
            {
                insertHashTable(createNewSymbolTableNode(newFuncType, node0->value, 2, isDefined, curDepth), Table);
            }
        }
        else
        {
            if (typeEqual(symbolTableNode->type, newFuncType) == 0)
            {
                printSemanticError(Conflict_Decordef_Funcion, root->lineNumber, node0->value);
            }
        }
    }
    else
    {
        insertHashTable(createNewSymbolTableNode(newFuncType, node0->value, 2, isDefined, curDepth), Table);
        if (isDefined == 0)
        {
            insertFunction(node0->value, root->lineNumber);
        }
    }
}

/* VarList 检查 */
FieldList VarList(AST *root, HashTableNode hashTableNode)
{
    AST *node0 = getChild(root, 0);
    AST *tmpNode = root;
    FieldList filedList = ParamDec(node0);
    Type type = (Type)(malloc(sizeof(struct Type_)));
    int isDefined;
    int flag = getLocalVariable(&type, filedList->name, &isDefined, 0, 0);
    if (flag && type != NULL && type->kind == STRUCTURE)
        printSemanticError(Redefined_Variable_Name, root->lineNumber, filedList->name);
    SymbolTableNode symbolTableNode = createNewSymbolTableNode(filedList->type, filedList->name, 0, 1, curDepth);
    insertHashTable(symbolTableNode, hashTableNode);
    FieldList f = filedList;
    while (TRUE)
    {
        if (getChild(tmpNode, 1) == NULL)
            break;
        tmpNode = getChild(tmpNode, 2);
        FieldList paramFiledList = ParamDec(getChild(tmpNode, 0));
        Type varType = (Type)(malloc(sizeof(struct Type_)));
        int isTmpDefined;
        int tmpFlag = getLocalVariable(&varType, paramFiledList->name, &isTmpDefined, 0, 0);
        if (tmpFlag && varType != NULL && varType->kind == STRUCTURE)
            printSemanticError(Redefined_Variable_Name, root->lineNumber, paramFiledList->name);
        symbolTableNode = createNewSymbolTableNode(paramFiledList->type, paramFiledList->name, 0, 1, curDepth);
        insertHashTable(symbolTableNode, hashTableNode);
        f->nextFieldList = paramFiledList;
        f = f->nextFieldList;
    }
    f->nextFieldList = NULL;
    return filedList;
}

/* ParamDec 检查 */
FieldList ParamDec(AST *root)
{
    /*
    ParamDec -> Specifier VarDec
    */
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    if (node0 != NULL && node1 != NULL)
    {
        Type type = Specifier(node0);
        return VarDec(node1, type);
    }
    return NULL;
}

/* CompSt 检查 */
void CompSt(AST *root, HashTableNode hashTableNode, Type type)
{
    /*
    CompSt -> LC DefList StmtList RC
    */
    AST *node1 = getChild(root, 1);
    AST *node2 = getChild(root, 2);
    if (stringComparison(node1->name, "DefList"))
    {
        DefList(node1, hashTableNode);
        if (stringComparison(node2->name, "StmtList"))
            StmtList(node2, hashTableNode, type);
    }
    else if (stringComparison(node1->name, "StmtList"))
        StmtList(node1, hashTableNode, type);
}

/* StmtList 检查 */
void StmtList(AST *root, HashTableNode hashTableNode, Type type)
{
    /*
    StmtList -> Stmt StmtList
    | (empty)
    */
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    if (node0 != NULL)
    {
        Stmt(node0, hashTableNode, type);
        if (node1 != NULL)
            StmtList(node1, hashTableNode, type);
    }
}

/* Stmt 检查 */
void Stmt(AST *root, HashTableNode hashTableNode, Type type)
{
    /*
    Stmt -> Exp SEMI
    | CompSt
    | RETURN Exp SEMI
    | IF LP Exp RP Stmt
    | IF LP Exp RP Stmt ELSE Stmt
    | WHILE LP Exp RP Stmt
    */
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    if (node0 != NULL)
    {
        if (stringComparison(node0->name, "Exp")) // Stmt -> Exp SEMI
        {
            Exp(node0);
        }
        else if (stringComparison(node0->name, "CompSt")) // Stmt -> CompSt
        {
            HashTableNode newHashTableNode = enterInnermostHashTable();
            curDepth++;
            CompSt(node0, newHashTableNode, type);
            curDepth--;
            deleteLocalVariable();
        }
        else if (stringComparison(node0->name, "RETURN")) // Stmt -> RETURN Exp SEMI
        {
            Type returnType = Exp(node1);
            if (returnType != NULL)
            {
                if (typeEqual(returnType, type) == FALSE)
                {
                    printSemanticError(Return_Type_Dismatch, root->lineNumber, NULL);
                }
            }
        }
        else if (stringComparison(node0->name, "WHILE")) // Stmt -> WHILE LP Exp RP Stmt
        {
            AST *node2 = getChild(root, 2);
            AST *node4 = getChild(root, 4);
            Type whileType = Exp(node2);
            if (whileType != NULL)
            {
                if (whileType->kind != BASIC || whileType->u.basic != 0)
                {
                    printSemanticError(Operand_Type_Dismatch, root->lineNumber, NULL);
                }
            }
            Stmt(node4, hashTableNode, type);
        }
        else if (stringComparison(node0->name, "IF"))
        {
            AST *node2 = getChild(root, 2);
            AST *node4 = getChild(root, 4);
            AST *node5 = getChild(root, 5);
            AST *node6 = getChild(root, 6);
            Type ifType = Exp(node2);
            if (ifType != NULL)
            {
                if (ifType->kind != BASIC || ifType->u.basic != 0)
                {
                    printSemanticError(Operand_Type_Dismatch, root->lineNumber, NULL);
                }
            }
            Stmt(node4, hashTableNode, type); // Stmt -> IF LP Exp RP Stmt
            if (node5 != NULL)                // Stmt -> IF LP Exp RP Stmt ELSE Stmt
            {
                Stmt(node6, hashTableNode, type);
            }
        }
    }
}

/* DefList 检查 */
void DefList(AST *root, HashTableNode hashTableNode)
{
    /*
    DefList -> Def DefList
    | (empty)
    */
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    if (node0 != NULL)
    {
        Def(node0, hashTableNode);
        if (node1 != NULL)
            DefList(node1, hashTableNode);
    }
}

/* Def 检查 */
void Def(AST *root, HashTableNode hashTableNode)
{
    /*
    Def -> Specifier DecList SEMI
    */
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    if (node0 != NULL && node1 != NULL)
    {
        Type type = Specifier(node0);
        DecList(node1, hashTableNode, type);
    }
}

/* DecList 检查 */
void DecList(AST *root, HashTableNode hashTableNode, Type type)
{
    /*
    DecList -> Dec
    | Dec COMMA DecList
    */
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    AST *node2 = getChild(root, 2);
    Dec(node0, hashTableNode, type);

    if (node1 != NULL && node2 != NULL) // DecList -> Dec COMMA DecList
    {
        DecList(node2, hashTableNode, type);
    }
}

/* Dec 检查 */
void Dec(AST *root, HashTableNode hashTableNode, Type type)
{
    /*
    Dec -> VarDec
    | VarDec ASSIGNOP Exp
    */
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    AST *node2 = getChild(root, 2);
    FieldList varDecField = VarDec(node0, type);
    int isDefined1 = 0;
    Type Type1 = (Type)malloc(sizeof(struct Type_));
    int localres = getLocalVariable(&Type1, varDecField->name, &isDefined1, curDepth, 0);
    int isDefined2 = 0;
    int kind = 0;
    Type Type2 = (Type)malloc(sizeof(struct Type_));
    int globalres = getGlobalVariable(&Type2, varDecField->name, &isDefined2, curDepth, &kind);
    if (node1 == NULL) // Dec -> VarDec
    {
        if (localres == 1)
        {
            printSemanticError(Redefined_Variable_Name, root->lineNumber, varDecField->name);
        }
        else if (globalres == 1 && Type2->kind == STRUCTURE && kind == 1)
        {
            printSemanticError(Redefined_Variable_Name, root->lineNumber, varDecField->name);
        }
        else
        {
            insertHashTable(createNewSymbolTableNode(varDecField->type, varDecField->name, 0, 1, curDepth), hashTableNode);
        }
    }
    else if (stringComparison(node1->name, "ASSIGNOP")) // Dec -> VarDec ASSIGNOP Exp
    {
        if (localres == 1)
        {
            printSemanticError(Redefined_Variable_Name, root->lineNumber, varDecField->name);
        }
        else
        {
            insertHashTable(createNewSymbolTableNode(varDecField->type, varDecField->name, 0, 1, curDepth), hashTableNode);
            Type expType = Exp(node2);
            if (expType != NULL)
            {
                if (typeEqual(varDecField->type, expType) == FALSE)
                {
                    printSemanticError(AssignOP_Type_Dismatch, root->lineNumber, NULL);
                }
                else if (globalres = 1 && Type2->kind == STRUCTURE && kind == 1)
                {
                    printSemanticError(Redefined_Variable_Name, root->lineNumber, varDecField->name);
                }
            }
        }
    }
}

/* Exp 检查 */
Type Exp(AST *root)
{
    /*
    Exp -> Exp ASSIGNOP Exp
    | Exp AND Exp
    | Exp OR Exp
    | Exp RELOP Exp
    | Exp PLUS Exp
    | Exp MINUS Exp
    | Exp STAR Exp
    | Exp DIV Exp
    | LP Exp RP
    | MINUS Exp
    | NOT Exp
    | ID LP Args RP
    | ID LP RP
    | Exp LB Exp RB
    | Exp DOT ID;
    | ID
    | INT
    | FLOAT
    */
    if (root == NULL)
        return NULL;
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    AST *node2 = getChild(root, 2);
    AST *node3 = getChild(root, 3);

    if (stringComparison(node0->name, "Exp"))
    {
        if (stringComparison(node1->name, "ASSIGNOP")) // Exp -> Exp ASSIGNOP Exp
        {
            AST *node00 = getChild(node0, 0);
            AST *node01 = getChild(node0, 1);
            AST *node02 = getChild(node0, 2);
            AST *node03 = getChild(node0, 3);
            if (node01 == NULL && !stringComparison(node00->name, "ID"))
            {
                printSemanticError(Leftside_Rvalue_Error, root->lineNumber, NULL);
                return NULL;
            }
            if (node01 != NULL)
            {
                if (node02 != NULL)
                {
                    if (node03 == NULL)
                    {
                        if (!stringComparison(node00->name, "Exp") || !stringComparison(node01->name, "DOT") || !stringComparison(node02->name, "ID"))
                        {
                            printSemanticError(Leftside_Rvalue_Error, root->lineNumber, NULL);
                            return NULL;
                        }
                    }
                    else if (!stringComparison(node00->name, "Exp") || !stringComparison(node01->name, "LB") || !stringComparison(node02->name, "Exp") || !stringComparison(node03->name, "RB"))
                    {
                        printSemanticError(Leftside_Rvalue_Error, root->lineNumber, NULL);
                        return NULL;
                    }
                }
                else
                {
                    printSemanticError(Leftside_Rvalue_Error, root->lineNumber, NULL);
                    return NULL;
                }
            }
            Type type1 = Exp(node0);
            Type type2 = Exp(node2);
            if (typeEqual(type1, type2) == FALSE)
            {
                printSemanticError(AssignOP_Type_Dismatch, root->lineNumber, NULL);
                return NULL;
            }
            return type1;
        }
        else if (stringComparison(node1->name, "AND") || stringComparison(node1->name, "OR") || stringComparison(node1->name, "RELOP") || stringComparison(node1->name, "PLUS") || stringComparison(node1->name, "MINUS") || stringComparison(node1->name, "STAR") || stringComparison(node1->name, "DIV"))
        {
            Type type1 = Exp(node0);
            Type type2 = Exp(node2);
            if (type1 == NULL || type2 == NULL)
                return NULL;
            else if (type1->u.basic == type2->u.basic)
            {
                return type1;
            }
            else
            {
                printSemanticError(Operand_Type_Dismatch, root->lineNumber, NULL);
                return NULL;
            }
        }
        else if (stringComparison(node1->name, "LB")) // Exp -> Exp LB Exp RB
        {
            Type type1 = Exp(node0);
            if (type1 == NULL)
                return NULL;
            if (type1->kind != ARRAY)
            {
                printSemanticError(Operate_Others_As_Array, root->lineNumber, NULL);
                return NULL;
            }
            Type type2 = Exp(node2);
            if (type2 == NULL)
                return NULL;
            if (!((type2->kind == BASIC || type2->kind == FUNCTION) && type2->u.basic == 0))
            {
                printSemanticError(Array_Float_Index, root->lineNumber, NULL);
                return NULL;
            }
            return type1->u.array.element;
        }
        else if (stringComparison(node1->name, "DOT")) // Exp -> Exp DOT ID
        {
            Type type1 = Exp(node0);
            if (type1 == NULL)
                return NULL;
            if (type1->kind != STRUCTURE)
            {
                printSemanticError(Operate_Others_As_Struct, root->lineNumber, NULL);
                return NULL;
            }
            FieldList field = type1->u.structure.structures;
            while (field != NULL)
            {
                if (stringComparison(field->name, node2->value))
                    return field->type;
                field = field->nextFieldList;
            }
            printSemanticError(Undefined_Field, root->lineNumber, node2->value);
            return NULL;
        }
    }
    else if (node1 == NULL)
    {
        if (stringComparison(node0->name, "ID"))
        {
            Type result = NULL;
            Type type1 = (Type)(malloc(sizeof(struct Type_)));
            int isDefined1 = 0;
            int localRes = getLocalVariable(&type1, node0->value, &isDefined1, curDepth, 0);
            Type type2 = (Type)(malloc(sizeof(struct Type_)));
            int kind = 0;
            int isDefined2 = 0;
            int globalRes = getGlobalVariable(&type2, node0->value, &isDefined2, curDepth, &kind);
            if (localRes == 1)
            {
                result = type1;
                return result;
            }
            else if (globalRes == 0 || (globalRes == 1 && kind != 0))
            {
                printSemanticError(Undefined_Variable, root->lineNumber, NULL);
                return NULL;
            }
            else
            {
                result = type2;
                return result;
            }
        }
        else if (stringComparison(node0->name, "INT"))
        {

            Type result = NULL;
            result = (Type)(malloc(sizeof(Type_)));
            result->kind = BASIC;
            result->u.basic = 0;
            return result;
        }
        else if (stringComparison(node0->name, "FLOAT"))
        {

            Type result = NULL;
            result = (Type)(malloc(sizeof(Type_)));
            result->kind = BASIC;
            result->u.basic = 1;
            return result;
        }
    }
    else
    {
        if (stringComparison(node0->name, "LP")) // LP Exp RP
        {
            return Exp(node1);
        }
        else if (stringComparison(node0->name, "MINUS")) // MINUS Exp
        {
            Type type = Exp(node1);
            if (type == NULL)
                return NULL;
            if (type->kind != BASIC)
            {
                printSemanticError(Operand_Type_Dismatch, root->lineNumber, NULL);
                return NULL;
            }
            return type;
        }
        else if (stringComparison(node0->name, "NOT")) // NOT Exp
        {
            Type type = Exp(node1);
            if (type == NULL)
                return NULL;
            if (type->kind == BASIC && type->u.basic == 0)
                return type;
            printSemanticError(Operand_Type_Dismatch, root->lineNumber, NULL);
            return NULL;
        }
        else if (stringComparison(node0->name, "ID"))
        {
            Type type = (Type)(malloc(sizeof(struct Type_)));
            int isDefined = 0;
            int localres = getLocalVariable(&type, node0->value, &isDefined, curDepth, 1);
            Type returnType = type->u.function.returnType;
            if (localres == 1 && type->kind != FUNCTION)
            {
                printSemanticError(Operate_Basic_As_Func, root->lineNumber, node0->value);
                return NULL;
            }
            if (localres == 0)
            {
                printSemanticError(Undefined_Function, root->lineNumber, node0->value);
                return NULL;
            }
            if (stringComparison(node2->name, "Args")) // ID LP Args RP
            {
                char *name = node0->value;
                Type findType = (Type)(malloc(sizeof(struct Type_)));
                int isDefined = -1;
                int flag = getLocalVariable(&findType, name, &isDefined, curDepth, 1);
                Type result = findType->u.function.returnType;
                if (flag == 1)
                {
                    if (findType->kind != FUNCTION)
                    {
                        printSemanticError(Operate_Basic_As_Func, root->lineNumber, name);
                        return NULL;
                    }
                }
                else
                {
                    printSemanticError(Undefined_Function, root->lineNumber, name);
                    return NULL;
                }
                if (findType->u.function.parameters == NULL)
                {

                    printSemanticError(Func_Call_Parameter_Dismatch, root->lineNumber, NULL);
                    return NULL;
                }
                else
                {
                    int layer = 0;
                    AST *curNode = node2;
                    while (TRUE)
                    {
                        layer++;
                        if (getChild(curNode, 1) == NULL)
                            break;
                        curNode = getChild(curNode, 2);
                    }
                    if (layer != findType->u.function.parameterNum)
                    {
                        printSemanticError(Func_Call_Parameter_Dismatch, root->lineNumber, NULL);
                        return NULL;
                    }
                    if (Args(node2, findType->u.function.parameters) == 0)
                        return result;
                    else
                        return NULL;
                }
            }
            else if (stringComparison(node2->name, "RP")) // ID LP RP
            {
                if (type->u.function.parameters != NULL)
                {
                    printSemanticError(Func_Call_Parameter_Dismatch, root->lineNumber, NULL);
                    return NULL;
                }
                return returnType;
            }
        }
    }
    return NULL;
}

/* Args 检查 */
int Args(AST *root, FieldList parameters)
{
    /*
    Args -> Exp COMMA Args
    | Exp;
    */
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    AST *node2 = getChild(root, 2);
    if (parameters == NULL)
    {
        printSemanticError(Func_Call_Parameter_Dismatch, root->lineNumber, NULL);
        return -1;
    }
    if (node0 != NULL)
    {
        Type expType = Exp(node0);
        if (expType != NULL)
        {
            if (parameters->type == NULL || typeEqual(expType, parameters->type) == FALSE)
            {
                printSemanticError(Func_Call_Parameter_Dismatch, root->lineNumber, NULL);
                return -1;
            }
        }
        if (node1 != NULL) // Args -> Exp COMMA Args
        {
            if (parameters->nextFieldList == NULL)
            {

                printSemanticError(Func_Call_Parameter_Dismatch, root->lineNumber, NULL);
                return -1;
            }
            else
                return Args(node2, parameters->nextFieldList);
        }
    }
    return 0;
}

/* StructDef 检查 */
FieldList StructDef(AST *root, char *name)
{
    /*
    Def -> Specifier DecList SEMI
    DecList -> Dec
        | Dec COMMA DecList
    */
    AST *node1 = getChild(root, 1);
    AST *tmpNode = node1;
    AST *node0 = getChild(root, 0);
    Type type = Specifier(node0);
    FieldList resField = NULL, tmpField = NULL;
    while (getChild(node1, 1) != NULL)
    {
        AST *node01 = getChild(node1, 0);
        FieldList decField = StructDec(node01, type);
        char *decName = (char *)malloc(1 + strlen(name) + strlen(decField->name));
        strcpy(decName, decField->name);
        strcat(decName, name);
        Type tmpType = (Type)malloc(sizeof(struct Type_));
        if (isExistStruct(&tmpType, decName))
            printSemanticError(Redefined_Field, node01->lineNumber, decField->name);
        else
            insertStruct(decField->type, decName);
        if (resField == NULL)
        {
            resField = decField;
            tmpField = resField;
        }
        else
        {
            tmpField->nextFieldList = decField;
            tmpField = tmpField->nextFieldList;
        }
        node1 = getChild(node1, 2);
    }
    AST *decNode = getChild(tmpNode, 0);
    FieldList decField = StructDec(decNode, type);
    char *decName = (char *)malloc(strlen(name) + strlen(decField->name) + 1);
    strcpy(decName, decField->name);
    strcat(decName, name);
    Type nulltype = (Type)malloc(sizeof(struct Type_));
    if (isExistStruct(&nulltype, decName))
        printSemanticError(Redefined_Field, decNode->lineNumber, decField->name);
    else
        insertStruct(decField->type, decName);
    if (resField == NULL)
    {
        resField = decField;
        tmpField = resField;
    }
    else
    {
        tmpField->nextFieldList = decField;
        tmpField = tmpField->nextFieldList;
    }
    return resField;
}

/* StructDec 检查 */
FieldList StructDec(AST *root, Type type)
{
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    FieldList field = VarDec(node0, type);
    if (node1 != NULL)
    {
        printSemanticError(Redefined_Field, root->lineNumber, field->name);
    }
    return field;
}