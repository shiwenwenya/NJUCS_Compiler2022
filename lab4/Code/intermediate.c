#include "intermediate.h"
int varNo = 1;
int tempNo = 1;
int labelNo = 1;
InterCodes interCodeListHead = NULL;
InterCodes interCodeListTail = NULL;
// 开始中间代码生成
/* translateProgram Program翻译 */
void translateProgram(AST *root, FILE *file)
{
    /* 初始化双向链表 */
    interCodeListHead = (InterCodes)malloc(sizeof(struct InterCodes_));
    interCodeListHead->next = NULL;
    interCodeListHead->prev = NULL;
    interCodeListTail = interCodeListHead;
    /*
    Program -> ExfDefList
    */
    translateExtDefList(getChild(root, 0));
    // printInterCodes(file); // 打印中间代码
}

/* translateExtDefList  ExtDefList翻译 */
void translateExtDefList(AST *root)
{
    /*
    ExfDefList -> ExfDef ExfDefList
    | (empty)
    */
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    if (node0 != NULL && node1 != NULL) // ExfDefList -> ExfDef ExfDefList
    {
        translateExtDef(node0);
        translateExtDefList(node1);
    }
    if (node0 != NULL && node1 == NULL) // ExfDefList -> ExfDef
    {
        translateExtDef(node0);
    }
}

/* translateExtDef ExtDef翻译 */
void translateExtDef(AST *root)
{
    /*
    ExtDef -> Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
    | Specifier FunDec SEMI
    */
    AST *node1 = getChild(root, 1);
    AST *node2 = getChild(root, 2);
    if (stringComparison(node1->name, "ExtDecList"))
    {
        translateExtDecList(node1);
    }
    else if (stringComparison(node1->name, "FunDec"))
    {
        if (stringComparison(node2->name, "CompSt"))
        {
            translateFunDec(node1);
            translateCompSt(node2);
        }
    }
}

/* translateExtDecList ExtDecList翻译 */
void translateExtDecList(AST *root)
{
    /*
    ExtDecList -> VarDec
    | VarDec COMMA ExtDecList
    */
    AST *node0 = getChild(root, 0);
    AST *node2 = getChild(root, 2);
    if (node0 != NULL)
    {
        translateVarDec(node0);
        if (node2 != NULL)
            translateExtDecList(node2);
    }
}

/* translateVarDec VarDec翻译 */
Operand translateVarDec(AST *root)
{
    /*
    VarDec -> ID
    | VarDec LB INT RB
    */
    Operand result = NULL;
    if (root == NULL)
    {
        return NULL;
    }
    AST *node0 = getChild(root, 0);
    if (stringComparison(node0->name, "ID"))
    {
        SymbolTableNode symbolTableNode = getSymbolTableNode(node0->value, __INT_MAX__);
        result = createOperand(VARIABLE_OP, VAL, node0->value);
        symbolTableNode->isAddress = result->type;
        symbolTableNode->var_no = result->var_no;
        if (getTypeSize(symbolTableNode->type) != 4)
        {
            Operand operand = createOperand(CONSTANT_OP, VAL, getTypeSize(symbolTableNode->type));
            createInterCodes(DEC_InterCode, result, operand);
        }
    }
    else
    {
        AST *node00 = getChild(node0, 0);
        while (!stringComparison(node00->name, "ID"))
        {
            node00 = node00->firstChildrenNode;
        }
        SymbolTableNode symbolTableNode = getSymbolTableNode(node00->value, __INT_MAX__);
        result = createOperand(VARIABLE_OP, VAL, node00->value);
        symbolTableNode->isAddress = result->type;
        symbolTableNode->var_no = result->var_no;
        Operand operand = createOperand(CONSTANT_OP, VAL, getTypeSize(symbolTableNode->type));
        createInterCodes(DEC_InterCode, result, operand);
    }
    return result;
}

/* translateFunDec FunDec翻译 */
void translateFunDec(AST *root)
{
    /*
    FunDec -> ID LP VarList RP
    | ID LP RP
    */
    AST *node0 = getChild(root, 0);
    Operand operand = createOperand(FUNCTION_OP, VAL, node0->value);
    createInterCodes(FUNC_InterCode, operand);
    SymbolTableNode symbolTableNode = getSymbolTableNode(operand->funcName, __INT_MAX__);
    if (symbolTableNode == NULL)
    {
        return;
    }
    if (symbolTableNode->type->u.function.parameterNum != 0)
    {
        FieldList parameters = symbolTableNode->type->u.function.parameters;
        while (parameters != NULL)
        {
            Operand tmpOperand = NULL;
            if (parameters->type->kind == ARRAY || parameters->type->kind == STRUCTURE)
            {
                tmpOperand = createOperand(VARIABLE_OP, ADDRESS, parameters->name);
            }
            else
            {
                tmpOperand = createOperand(VARIABLE_OP, VAL, parameters->name);
            }
            SymbolTableNode queryParameters = getSymbolTableNode(parameters->name, __INT_MAX__);
            if (queryParameters == NULL)
            {
                return;
            }
            queryParameters->var_no = tmpOperand->var_no;
            queryParameters->isAddress = tmpOperand->type;
            tmpOperand->type = VARIABLE_OP;
            createInterCodes(PARAM_InterCode, tmpOperand);
            parameters = parameters->nextFieldList;
        }
    }
}

/* translateVarList VarList翻译 */
void translateVarList(AST *root)
{
    /*
    VarList -> ParamDec COMMA VarList
    | ParamDec
    */
    AST *node0 = getChild(root, 0);
    translateParamDec(node0);
    AST *node2 = getChild(root, 2);
    if (node2 != NULL)
    {
        translateVarList(node2);
    }
}

/* translateParamDec ParamDec翻译 */
void translateParamDec(AST *root)
{
    /*
    ParamDec -> Specifier VarDec
    */
    AST *node1 = getChild(root, 1);
    if (node1 != NULL)
    {
        translateVarDec(node1);
    }
}

/* translateCompSt CompSt翻译 */
void translateCompSt(AST *root)
{
    /*
    CompSt -> LC DefList StmtList RC
    */
    AST *node1 = getChild(root, 1);
    AST *node2 = getChild(root, 2);
    if (stringComparison(node1->name, "DefList"))
    {
        translateDefList(node1);
        if (stringComparison(node2->name, "StmtList"))
            translateStmtList(node2);
    }
    else if (stringComparison(node1->name, "StmtList"))
        translateStmtList(node1);
}

/* translateStmtList StmtList翻译 */
void translateStmtList(AST *root)
{
    /*
    StmtList -> Stmt StmtList
    | (empty)
    */
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    if (node0 != NULL)
    {
        translateStmt(node0);
        if (node1 != NULL)
        {
            translateStmtList(node1);
        }
    }
}

/* translateStmt Stmt翻译 */
void translateStmt(AST *root)
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
    AST *node2 = getChild(root, 2);
    AST *node4 = getChild(root, 4);
    AST *node5 = getChild(root, 5);
    AST *node6 = getChild(root, 6);
    if (node0 != NULL)
    {
        if (stringComparison(node0->name, "Exp")) // Stmt -> Exp SEMI
        {
            translateExp(node0);
        }
        else if (stringComparison(node0->name, "CompSt")) // Stmt -> CompSt
        {
            translateCompSt(node0);
        }
        else if (stringComparison(node0->name, "RETURN")) // Stmt -> RETURN Exp SEMI
        {
            Operand op = translateExp(node1);
            createInterCodes(RETURN_InterCode, op);
        }
        else if (stringComparison(node0->name, "WHILE")) // Stmt -> WHILE LP Exp RP Stmt
        {
            Operand label1 = createOperand(LABEL_OP, VAL);
            Operand label2 = createOperand(LABEL_OP, VAL);
            createInterCodes(LABEL_InterCode, label1);
            translateCond(node2, NULL, label2);
            translateStmt(node4);
            createInterCodes(GOTO_InterCode, label1);
            createInterCodes(LABEL_InterCode, label2);
        }
        else if (stringComparison(node0->name, "IF"))
        {
            Operand label1 = createOperand(LABEL_OP, VAL);
            translateCond(node2, NULL, label1);
            translateStmt(node4);
            if (node5 == NULL) // IF LP Exp RP Stmt
            {
                createInterCodes(LABEL_InterCode, label1);
            }
            else // IF LP Exp RP Stmt ELSE Stmt
            {
                Operand label2 = createOperand(LABEL_OP, VAL);
                createInterCodes(GOTO_InterCode, label2);
                createInterCodes(LABEL_InterCode, label1);
                translateStmt(node6);
                createInterCodes(LABEL_InterCode, label2);
            }
        }
    }
}

/* translateDefList DefList翻译 */
void translateDefList(AST *root)
{
    /*
    DefList -> Def DefList
    | (empty)
    */
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    if (node0 != NULL)
    {
        translateDef(node0);
        if (node1 != NULL)
        {
            translateDefList(node1);
        }
    }
}

/* translateDefList Def翻译 */
void translateDef(AST *root)
{
    /*
    Def -> Specifier DecList SEMI
    */
    AST *node1 = getChild(root, 1);
    if (node1 != NULL)
    {
        translateDecList(node1);
    }
}

/* translateDecList DecList翻译 */
void translateDecList(AST *root)
{
    /*
    DecList -> Dec
    | Dec COMMA DecList
    */
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    AST *node2 = getChild(root, 2);
    translateDec(node0);
    if (node1 != NULL && node2 != NULL) // DecList -> Dec COMMA DecList
    {
        translateDecList(node2);
    }
}

/* translateDec Dec翻译 */
void translateDec(AST *root)
{
    /*
    Dec -> VarDec
    | VarDec ASSIGNOP Exp
    */
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    AST *node2 = getChild(root, 2);
    if (node1 == NULL)
    {
        translateVarDec(node0);
    }
    else
    {
        Operand op1 = translateVarDec(node0);
        Operand op2 = translateExp(node2);
        createInterCodes(ASSIGN_InterCode, op1, op2);
    }
}

/* translateExp Exp翻译 */
Operand translateExp(AST *root)
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
    | Exp DOT ID
    | ID
    | INT
    | FLOAT
    */
    Operand returnOperand = NULL;
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    AST *node2 = getChild(root, 2);
    if (node0 == NULL)
        return NULL;
    if (stringComparison(node0->name, "Exp"))
    {
        if (stringComparison(node1->name, "ASSIGNOP")) // Exp ASSIGNOP Exp
        {
            Operand op1 = translateExp(node0);
            Operand op2 = translateExp(node2);

            int flag = 0;
            if (op1->varName != NULL && op2->varName != NULL)
            {
                SymbolTableNode symbol1 = getSymbolTableNode(op1->varName, __INT_MAX__);
                SymbolTableNode symbol2 = getSymbolTableNode(op2->varName, __INT_MAX__);
                if (symbol1->type->kind == ARRAY && symbol2->type->kind == ARRAY && op1->type == VAL && op2->type == VAL)
                {
                    flag = 1;
                }
            }
            if (flag == 1)
            {
                SymbolTableNode symbol1 = getSymbolTableNode(op1->varName, __INT_MAX__);
                SymbolTableNode symbol2 = getSymbolTableNode(op2->varName, __INT_MAX__);
                int depth1 = op1->depth;
                int depth2 = op2->depth;
                int typesize = getTypeSize(symbol2->type);
                Operand constantOperand = createOperand(CONSTANT_OP, VAL, typesize);
                Operand constantOperandFour = createOperand(CONSTANT_OP, VAL, 4);
                Operand copy1 = copyOperand(op1);
                Operand copy2 = copyOperand(op2);
                if (copy1->kind == VARIABLE_OP)
                {
                    copy1->type = ADDRESS;
                }
                if (copy2->kind == VARIABLE_OP)
                {
                    copy2->type = ADDRESS;
                }
                Operand tempOperand1 = createOperand(TEMP_OP, VAL);
                createInterCodes(ASSIGN_InterCode, tempOperand1, copy1);
                Operand tempOperand2 = createOperand(TEMP_OP, VAL);
                createInterCodes(ASSIGN_InterCode, tempOperand2, copy2);
                Operand variableOperand = createOperand(VARIABLE_OP, ADDRESS, op2->varName);
                variableOperand->var_no = symbol2->var_no;
                Operand endOperand = createOperand(TEMP_OP, VAL);
                createInterCodes(ADD_InterCode, endOperand, variableOperand, constantOperand);
                Operand labelOperand1 = createOperand(LABEL_OP, VAL);
                Operand labelOperand2 = createOperand(LABEL_OP, VAL);
                createInterCodes(LABEL_InterCode, labelOperand1);
                createInterCodes(LABEL_InterCode, tempOperand2, ">=", endOperand, labelOperand2);
                Operand copytempOperand1 = copyOperand(tempOperand1);
                Operand copytempOperand2 = copyOperand(tempOperand2);
                copytempOperand1->type = ADDRESS;
                copytempOperand2->type = ADDRESS;
                createInterCodes(ASSIGN_InterCode, copytempOperand1, copytempOperand2);
                createInterCodes(ADD_InterCode, tempOperand1, tempOperand1, constantOperandFour);
                createInterCodes(ADD_InterCode, tempOperand2, tempOperand2, constantOperandFour);
                createInterCodes(GOTO_InterCode, labelOperand1);
                createInterCodes(LABEL_InterCode, labelOperand2);
            }
            else if (op1 != NULL && op2 != NULL)
            {
                createInterCodes(ASSIGN_InterCode, op1, op2);
            }
            returnOperand = op1;
            return returnOperand;
        }
        else if (stringComparison(node1->name, "AND") || stringComparison(node1->name, "OR") || stringComparison(node1->name, "RELOP")) // Exp AND Exp | Exp OR Exp | Exp RELOP Exp
        {
            Operand label1 = createOperand(LABEL_OP, VAL);
            Operand label2 = createOperand(LABEL_OP, VAL);
            returnOperand = createOperand(TEMP_OP, VAL);
            Operand newOperand = createOperand(CONSTANT_OP, VAL, 0);
            createInterCodes(ASSIGN_InterCode, returnOperand, newOperand);
            translateCond(root, label1, label2);
            createInterCodes(LABEL_InterCode, label1);
            Operand oneOperand = createOperand(CONSTANT_OP, VAL, 1);
            createInterCodes(ASSIGN_InterCode, returnOperand, oneOperand);
            createInterCodes(LABEL_InterCode, label2);
            return returnOperand;
        }
        else if (stringComparison(node1->name, "PLUS") || stringComparison(node1->name, "MINUS") || stringComparison(node1->name, "STAR") || stringComparison(node1->name, "DIV"))
        {
            int kind;
            if (stringComparison(node1->name, "PLUS")) // Exp PLUS Exp
            {
                kind = ADD_InterCode;
            }
            else if (stringComparison(node1->name, "MINUS")) // Exp MINUS Exp
            {
                kind = SUB_InterCode;
            }
            else if (stringComparison(node1->name, "STAR")) // Exp STAR Exp
            {
                kind = MUL_InterCode;
            }
            else // Exp DIV Exp
            {
                kind = DIV_InterCode;
            }
            returnOperand = createOperand(TEMP_OP, VAL);
            Operand op1 = translateExp(node0);
            Operand op2 = translateExp(node2);
            if (op1 != NULL && op2 != NULL)
                createInterCodes(kind, returnOperand, op1, op2);
            return returnOperand;
        }
        else if (stringComparison(node1->name, "LB")) // Exp LB Exp RB
        {
            Operand copyExp = copyOperand(translateExp(node0));
            int depth = copyExp->depth;
            SymbolTableNode symbol = getSymbolTableNode(copyExp->varName, __INT_MAX__);
            Type tempType = symbol->type;
            int count = 0;
            while (tempType->kind == ARRAY)
            {
                count += 1;
                tempType = tempType->u.array.element;
            }
            int typeSize = getTypeSize(tempType);
            int *arraySize = (int *)malloc(sizeof(int) * (count + 1));
            count = 0;
            tempType = symbol->type;
            while (tempType->kind == ARRAY)
            {
                arraySize[count] = tempType->u.array.size;
                count += 1;
                tempType = tempType->u.array.element;
            }
            int index = count - 1;
            int tempDepth = count - depth - 1;
            int offset = 1;
            while (tempDepth)
            {
                offset *= arraySize[index];
                tempDepth -= 1;
                index -= 1;
            }
            free(arraySize);
            offset = offset * typeSize;
            Operand expOperand = translateExp(node2);
            Operand tempOperand1 = createOperand(TEMP_OP, VAL);
            Operand constantop1 = createOperand(CONSTANT_OP, VAL, offset);
            createInterCodes(MUL_InterCode, tempOperand1, expOperand, constantop1);
            Operand tempOperand2 = createOperand(TEMP_OP, VAL);
            tempOperand2->varName = copyExp->varName;
            tempOperand2->depth = depth + 1;
            if (depth == 0 && copyExp->type == VAL)
            {
                copyExp->type = ADDRESS;
            }
            else
            {
                copyExp->type = VAL;
            }
            createInterCodes(ADD_InterCode, tempOperand2, copyExp, tempOperand1);
            returnOperand = copyOperand(tempOperand2);
            if (tempOperand2->depth == count)
            {
                returnOperand->type = ADDRESS;
            }
            return returnOperand;
        }
        else if (stringComparison(node1->name, "DOT")) // Exp DOT ID
        {
            Operand expOperand = translateExp(node0);
            Operand copyExpOperand = copyOperand(expOperand);
            SymbolTableNode symbol = getSymbolTableNode(node2->value, __INT_MAX__);
            int offset = symbol->offset;
            if (offset == 0)
            {
                Operand tempOperand = createOperand(TEMP_OP, VAL);
                if (copyExpOperand->type == ADDRESS)
                {
                    copyExpOperand->type = VAL;
                }
                else
                {
                    copyExpOperand->type = ADDRESS;
                }
                createInterCodes(ASSIGN_InterCode, tempOperand, copyExpOperand);
                returnOperand = copyOperand(tempOperand);
                returnOperand->type = ADDRESS;
                returnOperand->varName = node2->value;
                return returnOperand;
            }
            else
            {
                Operand constantOperand = createOperand(CONSTANT_OP, VAL, offset);
                Operand tempOperand = createOperand(TEMP_OP, VAL);
                if (copyExpOperand->type == ADDRESS)
                {
                    copyExpOperand->type = VAL;
                }
                else
                {
                    copyExpOperand->type = ADDRESS;
                }
                createInterCodes(ADD_InterCode, tempOperand, copyExpOperand, constantOperand);
                returnOperand = copyOperand(tempOperand);
                returnOperand->type = ADDRESS;
                returnOperand->varName = node2->value;
                return returnOperand;
            }
        }
    }
    else if (stringComparison(node0->name, "LP"))
    {
        return translateExp(node1);
    }
    else if (stringComparison(node0->name, "MINUS")) // MINUS Exp
    {
        Operand newOperand = createOperand(CONSTANT_OP, VAL, 0);
        Operand op1 = translateExp(node1);
        Operand op2 = createOperand(TEMP_OP, VAL);
        createInterCodes(SUB_InterCode, op2, newOperand, op1);
        returnOperand = op2;
        return returnOperand;
    }
    else if (stringComparison(node0->name, "NOT")) // NOT Exp
    {
        Operand label1 = createOperand(LABEL_OP, VAL);
        Operand label2 = createOperand(LABEL_OP, VAL);
        returnOperand = createOperand(TEMP_OP, VAL);
        Operand newOperand = createOperand(CONSTANT_OP, VAL, 0);
        createInterCodes(ASSIGN_InterCode, returnOperand, newOperand);
        translateCond(root, label1, label2);
        createInterCodes(LABEL_InterCode, label1);
        Operand oneOperand = createOperand(CONSTANT_OP, VAL, 1);
        createInterCodes(ASSIGN_InterCode, returnOperand, oneOperand);
        createInterCodes(LABEL_InterCode, label2);
        return returnOperand;
    }
    else if (stringComparison(node0->name, "ID"))
    {
        if (node1 == NULL) // ID
        {
            SymbolTableNode idNode = getSymbolTableNode(node0->value, __INT_MAX__);
            if (idNode->type->kind == ARRAY || idNode->type->kind == STRUCTURE)
            {
                if (idNode->isAddress == ADDRESS)
                {
                    returnOperand = createOperand(VARIABLE_OP, ADDRESS, node0->value);
                }
                else
                {
                    returnOperand = createOperand(VARIABLE_OP, VAL, node0->value);
                }
                returnOperand->var_no = idNode->var_no;
                returnOperand->depth = 0;
                returnOperand->varName = node0->value;
                return returnOperand;
            }
            else
            {
                returnOperand = createOperand(VARIABLE_OP, VAL, node0->value);
                returnOperand->var_no = idNode->var_no;
                returnOperand->depth = 0;
                returnOperand->varName = node0->value;
                return returnOperand;
            }
        }
        else
        {
            if (stringComparison(node0->value, "write") && stringComparison(node2->name, "Args"))
            { // ID LP Args RP
                AST *expNode = getChild(node2, 0);
                Operand tempOperand = NULL;
                if (stringComparison(expNode->name, "Exp"))
                {
                    tempOperand = translateExp(expNode);
                }
                if (tempOperand != NULL)
                {
                    createInterCodes(WRITE_InterCode, tempOperand);
                }
                Operand constantOperand = createOperand(CONSTANT_OP, VAL, 0);
                returnOperand = createOperand(TEMP_OP, VAL);
                createInterCodes(ASSIGN_InterCode, returnOperand, constantOperand);
                return returnOperand;
            }
            returnOperand = createOperand(TEMP_OP, VAL);
            if (stringComparison(node0->value, "read"))
            {
                createInterCodes(READ_InterCode, returnOperand);
                return returnOperand;
            }
            Operand functionName = createOperand(FUNCTION_OP, VAL, node0->value);
            if (stringComparison(node2->name, "RP"))
            {
                createInterCodes(CALL_InterCode, returnOperand, functionName);
                return returnOperand;
            }
            else if (stringComparison(node2->name, "Args"))
            {
                SymbolTableNode idNode = getSymbolTableNode(node0->value, __INT_MAX__);
                translateArgs(node2, idNode->type->u.function.parameters);
                createInterCodes(CALL_InterCode, returnOperand, functionName);
                return returnOperand;
            }
            return returnOperand;
        }
    }
    else if (stringComparison(node0->name, "INT")) // INT
    {
        returnOperand = createOperand(CONSTANT_OP, VAL, My_atoi(node0->value));
        return returnOperand;
    }
    else if (stringComparison(node0->name, "FLOAT")) // FLOAT
    {
        returnOperand = createOperand(CONSTANT_OP, VAL, 0);
        return returnOperand;
    }
    return returnOperand;
}

/* translateArgs Args翻译 */
void translateArgs(AST *root, FieldList field)
{
    /*
    Args -> Exp COMMA Args
    | Exp;
    */
    if (root == NULL || field == NULL)
    {
        return;
    }
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    AST *node2 = getChild(root, 2);
    Operand expOperand = translateExp(node0);
    Operand tmpExpOperand = copyOperand(expOperand);
    if (field->type->kind == STRUCTURE || field->type->kind == ARRAY)
    {
        int flag = 0;
        if (field->type->kind == ARRAY)
        {
            char *name = tmpExpOperand->varName;
            SymbolTableNode arrayNode = getSymbolTableNode(name, __INT_MAX__);
            int count = 0;
            Type tmpType = arrayNode->type;
            while (tmpType->kind == ARRAY)
            {
                count++;
                tmpType = tmpType->u.array.element;
            }
            if (tmpExpOperand->depth < count)
            {
                flag = 1;
            }
            if (tmpExpOperand->depth == 0)
            {
                flag = 0;
            }
        }
        if (flag == 1)
        {
            tmpExpOperand->type = VAL;
        }
        else if (tmpExpOperand->type == ADDRESS)
        {
            tmpExpOperand->type = VAL;
        }
        else
        {
            tmpExpOperand->type = ADDRESS;
        }
    }
    if (node1 != NULL)
    {
        translateArgs(node2, field->nextFieldList);
    }
    createInterCodes(ARG_InterCode, tmpExpOperand);
}

/* translateCond Cond翻译 */
void translateCond(AST *root, Operand lableTure, Operand lableFalse)
{
    Operand newOperand = createOperand(CONSTANT_OP, VAL, 0);
    AST *node0 = getChild(root, 0);
    AST *node1 = getChild(root, 1);
    AST *node2 = getChild(root, 2);
    if (node0 != NULL)
    {
        if (stringComparison(node0->name, "Exp"))
        {
            if (stringComparison(node1->name, "ASSIGNOP"))
            {
                Operand op1 = translateExp(node0);
                Operand op2 = translateExp(node2);
                createInterCodes(ASSIGN_InterCode, op1, op2);
                if (lableTure != NULL && lableFalse != NULL)
                {
                    if (op1 != NULL)
                    {
                        createInterCodes(IFGOTO_InterCode, op1, "!=", newOperand, lableTure);
                    }
                    createInterCodes(GOTO_InterCode, lableFalse);
                }
                else if (lableTure != NULL)
                {
                    if (op1 != NULL)
                    {
                        createInterCodes(IFGOTO_InterCode, op1, "!=", newOperand, lableTure);
                    }
                }
                else if (lableFalse != NULL)
                {
                    if (op1 != NULL)
                    {
                        createInterCodes(IFGOTO_InterCode, op1, "==", newOperand, lableFalse);
                    }
                }
            }
            else if (stringComparison(node1->name, "AND"))
            {
                if (lableFalse != NULL)
                {
                    translateCond(node0, NULL, lableFalse);
                    translateCond(node2, lableTure, lableFalse);
                }
                else
                {
                    Operand label = createOperand(LABEL_OP, VAL);
                    translateCond(node0, NULL, label);
                    translateCond(node2, lableTure, lableFalse);
                    createInterCodes(LABEL_InterCode, label);
                }
            }
            else if (stringComparison(node1->name, "OR"))
            {
                if (lableTure != NULL)
                {
                    translateCond(node0, lableTure, NULL);
                    translateCond(node2, lableTure, lableFalse);
                }
                else
                {
                    Operand label = createOperand(LABEL_OP, VAL);
                    translateCond(node0, label, NULL);
                    translateCond(node2, lableTure, lableFalse);
                    createInterCodes(LABEL_InterCode, label);
                }
            }
            else if (stringComparison(node1->name, "RELOP"))
            {
                Operand op1 = translateExp(node0);
                Operand op2 = translateExp(node2);
                if (lableTure != NULL && lableFalse != NULL)
                {
                    if (op1 != NULL)
                    {
                        createInterCodes(IFGOTO_InterCode, op1, node1->value, op2, lableTure);
                    }
                    createInterCodes(GOTO_InterCode, lableFalse);
                }
                else if (lableTure != NULL)
                {
                    if (op1 != NULL)
                    {
                        createInterCodes(IFGOTO_InterCode, op1, node1->value, op2, lableTure);
                    }
                }
                else if (lableFalse != NULL)
                {
                    if (op1 != NULL)
                    {
                        createInterCodes(IFGOTO_InterCode, op1, NegateRelop(node1->value), op2, lableFalse);
                    }
                }
            }
            else if (stringComparison(node1->name, "PLUS") || stringComparison(node1->name, "MINUS") || stringComparison(node1->name, "STAR") || stringComparison(node1->name, "DIV"))
            {
                Operand op1 = translateExp(node0);
                Operand op2 = translateExp(node2);
                int kind;
                if (stringComparison(node1->name, "PLUS"))
                {
                    kind = ADD_InterCode;
                }
                else if (stringComparison(node1->name, "MINUS"))
                {
                    kind = SUB_InterCode;
                }
                else if (stringComparison(node1->name, "STAR"))
                {
                    kind = MUL_InterCode;
                }
                else
                {
                    kind = DIV_InterCode;
                }
                Operand result = createOperand(TEMP_OP, VAL);
                if (op1 != NULL && op2 != NULL)
                {
                    createInterCodes(kind, result, op1, op2);
                }
                if (lableTure != NULL && lableFalse != NULL)
                {
                    createInterCodes(IFGOTO_InterCode, result, "!=", newOperand, lableTure);
                    createInterCodes(GOTO_InterCode, lableFalse);
                }
                else if (lableTure != NULL)
                {
                    createInterCodes(IFGOTO_InterCode, result, "!=", newOperand, lableTure);
                }
                else if (lableFalse != NULL)
                {
                    createInterCodes(IFGOTO_InterCode, result, "==", newOperand, lableFalse);
                }
            }
            else if (stringComparison(node1->name, "LB"))
            {
                Operand op = translateExp(root);
                if (lableTure != NULL && lableFalse != NULL)
                {
                    createInterCodes(IFGOTO_InterCode, op, "!=", newOperand, lableTure);
                    createInterCodes(GOTO_InterCode, lableFalse);
                }
                else if (lableTure != NULL)
                {
                    createInterCodes(IFGOTO_InterCode, op, "!=", newOperand, lableTure);
                }
                else if (lableFalse != NULL)
                {
                    createInterCodes(IFGOTO_InterCode, op, "==", newOperand, lableFalse);
                }
            }
            else if (stringComparison(node1->name, "DOT"))
            {
                Operand op = translateExp(root);
                if (lableTure != NULL && lableFalse != NULL)
                {
                    createInterCodes(IFGOTO_InterCode, op, "!=", newOperand, lableTure);
                    createInterCodes(GOTO_InterCode, lableFalse);
                }
                else if (lableTure != NULL)
                {
                    createInterCodes(IFGOTO_InterCode, op, "!=", newOperand, lableTure);
                }
                else if (lableFalse != NULL)
                {
                    createInterCodes(IFGOTO_InterCode, op, "==", newOperand, lableFalse);
                }
            }
        }
        else if (stringComparison(node0->name, "LP"))
        {
            translateCond(node1, lableTure, lableFalse);
        }
        else if (stringComparison(node0->name, "MINUS"))
        {
            Operand op = translateExp(root);
            if (lableTure != NULL && lableFalse != NULL)
            {
                createInterCodes(IFGOTO_InterCode, op, "!=", newOperand, lableTure);
                createInterCodes(GOTO_InterCode, lableFalse);
            }
            else if (lableTure != NULL)
            {
                createInterCodes(IFGOTO_InterCode, op, "!=", newOperand, lableTure);
            }
            else if (lableFalse != NULL)
            {
                createInterCodes(IFGOTO_InterCode, op, "==", newOperand, lableFalse);
            }
        }
        else if (stringComparison(node0->name, "NOT"))
        {
            translateCond(node1, lableFalse, lableTure);
        }
        else if (stringComparison(node0->name, "ID"))
        {
            Operand op = translateExp(root);
            if (lableTure != NULL && lableFalse != NULL)
            {
                createInterCodes(IFGOTO_InterCode, op, "!=", newOperand, lableTure);
                createInterCodes(GOTO_InterCode, lableFalse);
            }
            else if (lableTure != NULL)
            {
                createInterCodes(IFGOTO_InterCode, op, "!=", newOperand, lableTure);
            }
            else if (lableFalse != NULL)
            {
                createInterCodes(IFGOTO_InterCode, op, "==", newOperand, lableFalse);
            }
        }
        else if (stringComparison(node0->name, "INT"))
        {
            if (lableTure != NULL && My_atoi(node0->value))
            {
                createInterCodes(GOTO_InterCode, lableTure);
            }
            if (lableFalse != NULL && !My_atoi(node0->value))
            {
                createInterCodes(GOTO_InterCode, lableFalse);
            }
        }
    }
}
