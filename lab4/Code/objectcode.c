#include "objectcode.h"
#define PRECODE ".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\n" \
                "read:\n\tli $v0, 4\n\tla $a0, _prompt\n\tsyscall\n\tli $v0, 5\n\tsyscall\n\tjr $ra\n\n"     \
                "write:\n\tli $v0, 1\n\tsyscall\n\tli $v0, 4\n\tla $a0, _ret\n\tsyscall\n\tmove $v0, $0\n\tjr $ra\n\n"

Register Registers[32];
char *RegName[32] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};

int localOffset = 0;
VarOffset varOffsetHead = NULL;

/* 目标代码生成 */
void generateObjectCode(FILE *file)
{
    initRegisters();        // 初始化寄存器
    fprintf(file, PRECODE); // 输出初始化目标代码
    InterCodes curInterCodes = interCodeListHead->next;
    while (curInterCodes != interCodeListHead) // 遍历中间代码，生成目标代码
    {
        switch (curInterCodes->code.kind)
        {
        case LABEL_InterCode:
        {
            generateLableObjectCode(curInterCodes, file);
            break;
        } // 定义标号x LABEL x :
        case FUNC_InterCode:
        {
            generateFuncObjectCode(curInterCodes, file);
            break;
        } // 定义函数f FUNCTION f :
        case ASSIGN_InterCode:
        {
            generateAssignObjectCode(curInterCodes, file);
            break;
        } // 赋值操作 x := y
        case ADD_InterCode:
        {
            generateAddObjectCode(curInterCodes, file);
            break;
        } // 加法操作 x := y + z
        case SUB_InterCode:
        {
            generateSubObjectCode(curInterCodes, file);
            break;
        } // 减法操作 x := y - z
        case MUL_InterCode:
        {
            generateMulObjectCode(curInterCodes, file);
            break;
        } // 乘法操作 x := y * z
        case DIV_InterCode:
        {
            generateDivObjectCode(curInterCodes, file);
            break;
        } // 除法操作 x := y / z
        case GET_ADDR_InterCode:
        {
            generateGetAddrObjectCode(curInterCodes, file);
            break;
        } // 取y的地址赋给x x := &y
        case GET_CONTENT_InterCode:
        {
            generateGetContentObjectCode(curInterCodes, file);
            break;
        } // 取以y值为地址的内存单元的内容赋给x x := *y
        case TO_ADDR_InterCode:
        {
            generateToAddrObjectCode(curInterCodes, file);
            break;
        } // 取y值赋给以x值为地址的内存单元 *x := y
        case GOTO_InterCode:
        {
            generateGotoObjectCode(curInterCodes, file);
            break;
        } // 无条件跳转至标号x GOTO x
        case IFGOTO_InterCode:
        {
            generateIfgotoObjectCode(curInterCodes, file);
            break;
        } // 如果x与y满足[relop]关系则跳转至标号z IF x [relop] y GOTO z
        case RETURN_InterCode:
        {
            generateReturnObjectCode(curInterCodes, file);
            break;
        } // 退出当前函数并返回x值 RETURN x
        case DEC_InterCode:
        {
            generateDecObjectCode(curInterCodes, file);
            break;
        } // 内存空间申请，大小为4的倍数 DEC x [size]
        case ARG_InterCode:
        {
            generateArgObjectCode(curInterCodes, file); // 所有的arg和call整体处理
            while (curInterCodes->code.kind != CALL_InterCode)
                curInterCodes = curInterCodes->next;
            break;
        } // 传实参x ARG x
        case CALL_InterCode:
        {
            generateParamObjectCode(curInterCodes, file);
            break;
        } // 调用函数，并将其返回值赋给x x := CALL f
        case PARAM_InterCode:
        {
            generateParamObjectCode(curInterCodes, file);
            break;
        } // 函数参数声明 PARAM x
        case READ_InterCode:
        {
            generateReadObjectCode(curInterCodes, file);
            break;
        } // 从控制台读取x的值 READ x
        case WRITE_InterCode:
        {
            generateWriteObjectCode(curInterCodes, file);
            break;
        } // 向控制台打印x的值 WRITE x
        }
        curInterCodes = curInterCodes->next;
    }
}

/* 初始化寄存器 */
void initRegisters()
{
    for (int i = 0; i < 32; i++)
        Registers[i].name = RegName[i];
    for (int i = 0; i < 32; i++)
    {
        Registers[i].used = 0;
        Registers[i].variable = NULL;
    }
}

/* 寻找合适寄存器并填入操作内容 */
int findRegisters(Operand op, FILE *file)
{
    if (op->kind == CONSTANT_OP)
    {
        for (int i = 8; i <= 15; i++)
        {
            if (Registers[i].used == 0)
            {
                Registers[i].used = 1;
                Registers[i].variable = NULL;
                fprintf(file, "\tli %s, %d\n", Registers[i].name, op->value);
                return i;
            }
        }
    }
    for (int i = 8; i <= 15; i++)
    {
        if (Registers[i].used == 0)
        {
            Registers[i].used = 1;
            VarOffset varOffset = getVarOffset(op);
            varOffset->reg = i;
            Registers[i].variable = varOffset;
            if (op->kind == TEMP_OP && op->type == ADDRESS)
            {
                fprintf(file, "\tlw %s, %d($fp)\n", Registers[i].name, varOffset->offset);
                fprintf(file, "\tlw %s, 0(%s)\n", Registers[i].name, Registers[i].name);
            }
            else if (op->kind == VARIABLE_OP && op->type == ADDRESS)
            {
                // fprintf(file, "\tla %s, %d($fp)\n", Registers[i].name, varOffset->offset);
                fprintf(file, "\taddi %s, $fp, %d\n", Registers[i].name, varOffset->offset);
            }
            else
            {
                fprintf(file, "\tlw %s, %d($fp)\n", Registers[i].name, varOffset->offset);
            }
            return i;
        }
    }
    return 0;
}

/* 获取操作数偏移量 */
VarOffset getVarOffset(Operand op)
{
    VarOffset curvarOffset = varOffsetHead;
    VarOffset ans = NULL;
    while (curvarOffset != NULL)
    {
        if (op->kind == VARIABLE_OP)
        {
            if (strcmp(curvarOffset->name, op->varName) == 0)
            {
                ans = curvarOffset;
                return ans;
            }
        }
        else if (op->kind == TEMP_OP)
        {
            char num[32];
            My_itoa(op->var_no, num);
            char name[32] = {'t'};
            strcat(name, num);
            if (strcmp(curvarOffset->name, name) == 0)
            {
                ans = curvarOffset;
                return ans;
            }
        }

        curvarOffset = curvarOffset->next;
    }
    return ans;
}

/* 将寄存器值存回原地址 */
void SaveRegister(int index, FILE *file)
{
    int offset = Registers[index].variable->offset;
    fprintf(file, "\tsw %s, %d($fp)\n", Registers[index].name, offset);
    for (int i = 8; i <= 15; i++)
        if (Registers[i].used == 1)
            Registers[i].used = 0;
}

/* 创建操作数偏移量 */
void createVarOffset(Operand op)
{
    if (op->kind == CONSTANT_OP)
    {
        return;
    }
    VarOffset tempvarOffset = getVarOffset(op);
    if (tempvarOffset == NULL)
    {
        localOffset += 4;
        VarOffset newvarOffset = (VarOffset)malloc(sizeof(VarOffset_));
        if (op->kind == VARIABLE_OP)
        {
            strcpy(newvarOffset->name, op->varName);
        }
        else if (op->kind == TEMP_OP)
        {
            char num[32];
            My_itoa(op->var_no, num);
            char name[32] = {'t'};
            strcat(name, num);
            strcpy(newvarOffset->name, name);
        }
        newvarOffset->offset = -localOffset;
        newvarOffset->next = varOffsetHead;
        varOffsetHead = newvarOffset;
    }
}

/* 目标代码生成：label */
void generateLableObjectCode(InterCodes curInterCodes, FILE *file)
{
    fprintf(file, "label%d:\n", curInterCodes->code.u.singleOP.op->var_no);
}

/* 目标代码生成：func */
void generateFuncObjectCode(InterCodes curInterCodes, FILE *file)
{
    fprintf(file, "%s:\n", curInterCodes->code.u.singleOP.op->funcName);
    fprintf(file, "\taddi $sp, $sp, -8\n");
    fprintf(file, "\tsw $fp, 0($sp)\n");
    fprintf(file, "\tsw $ra, 4($sp)\n");
    fprintf(file, "\tmove $fp, $sp\n");
    localOffset = 0;
    int paramCount = 0;

    InterCodes tmpInterCodes = curInterCodes;
    tmpInterCodes = tmpInterCodes->next;
    while (tmpInterCodes->code.kind == PARAM_InterCode)
    {
        VarOffset param = (VarOffset)malloc(sizeof(VarOffset_));
        strcpy(param->name, tmpInterCodes->code.u.singleOP.op->varName);
        param->offset = 8 + paramCount * 4;
        param->next = varOffsetHead;
        varOffsetHead = param;
        paramCount++;
        tmpInterCodes = tmpInterCodes->next;
    }
    while (tmpInterCodes != NULL && tmpInterCodes->code.kind != FUNC_InterCode) // 初始化偏移量
    {
        switch (tmpInterCodes->code.kind)
        {
        case ASSIGN_InterCode:
        {
            createVarOffset(tmpInterCodes->code.u.doubleOP.left);
            createVarOffset(tmpInterCodes->code.u.doubleOP.right);
        }
        break;
        case ADD_InterCode:
        case SUB_InterCode:
        case MUL_InterCode:
        case DIV_InterCode:
        {
            createVarOffset(tmpInterCodes->code.u.tripleOP.op1);
            createVarOffset(tmpInterCodes->code.u.tripleOP.op2);
            createVarOffset(tmpInterCodes->code.u.tripleOP.result);
        }
        break;
        case DEC_InterCode:
        {
            localOffset += tmpInterCodes->code.u.doubleOP.right->value;
            VarOffset array = (VarOffset)malloc(sizeof(VarOffset_));
            strcpy(array->name, tmpInterCodes->code.u.doubleOP.left->varName);
            array->offset = (-1) * localOffset;
            array->next = varOffsetHead;
            varOffsetHead = array;
        }
        break;
        case IFGOTO_InterCode:
        {
            createVarOffset(tmpInterCodes->code.u.ifgotoOP.op1);
            createVarOffset(tmpInterCodes->code.u.ifgotoOP.op2);
        }
        break;
        case CALL_InterCode:
        {
            createVarOffset(tmpInterCodes->code.u.doubleOP.left);
        }
        break;
        case ARG_InterCode:
        case WRITE_InterCode:
        case READ_InterCode:
        {
            createVarOffset(tmpInterCodes->code.u.singleOP.op);
        }
        break;
        default:
            break;
        }
        tmpInterCodes = tmpInterCodes->next;
    }
    fprintf(file, "\taddi $sp, $sp, %d\n", (-1) * localOffset);
    for (int i = 8; i <= 15; i++)
    {
        if (Registers[i].used == 1)
            Registers[i].used = 0;
    }
}

/* 目标代码生成：assign */
void generateAssignObjectCode(InterCodes curInterCodes, FILE *file)
{
    Operand leftOp = curInterCodes->code.u.doubleOP.left;
    Operand rightOp = curInterCodes->code.u.doubleOP.right;
    int rightIndex = findRegisters(rightOp, file);
    int leftIndex = 8;
    if (leftOp->kind == TEMP_OP && leftOp->type == ADDRESS) // *x = y
    {
        VarOffset leftVarOffset = getVarOffset(leftOp);
        for (int i = 8; i <= 15; i++)
            if (Registers[i].used == 0)
                leftIndex = i;
        Registers[leftIndex].used = 1;
        fprintf(file, "lw %s, %d($fp)\n", Registers[leftIndex].name, leftVarOffset->offset);
        fprintf(file, "sw %s, 0(%s)\n", Registers[rightIndex].name, Registers[leftIndex].name);
        Registers[leftIndex].used = 0;
        Registers[rightIndex].used = 0;
    }
    else // x
    {
        leftIndex = findRegisters(leftOp, file);
        fprintf(file, "\tmove %s, %s\n", Registers[leftIndex].name, Registers[rightIndex].name);
        SaveRegister(leftIndex, file);
    }
}

/* 目标代码生成：add */
void generateAddObjectCode(InterCodes curInterCodes, FILE *file)
{
    int resultIndex = findRegisters(curInterCodes->code.u.tripleOP.result, file);
    int op1Index = findRegisters(curInterCodes->code.u.tripleOP.op1, file);
    int op2Index = findRegisters(curInterCodes->code.u.tripleOP.op2, file);
    fprintf(file, "\tadd %s, %s, %s\n", Registers[resultIndex].name, Registers[op1Index].name, Registers[op2Index].name);
    SaveRegister(resultIndex, file);
}

/* 目标代码生成：sub */
void generateSubObjectCode(InterCodes curInterCodes, FILE *file)
{
    int resultIndex = findRegisters(curInterCodes->code.u.tripleOP.result, file);
    int op1Index = findRegisters(curInterCodes->code.u.tripleOP.op1, file);
    int op2Index = findRegisters(curInterCodes->code.u.tripleOP.op2, file);
    fprintf(file, "\tsub %s, %s, %s\n", Registers[resultIndex].name, Registers[op1Index].name, Registers[op2Index].name);
    SaveRegister(resultIndex, file);
}

/* 目标代码生成：mul */
void generateMulObjectCode(InterCodes curInterCodes, FILE *file)
{
    int resultIndex = findRegisters(curInterCodes->code.u.tripleOP.result, file);
    int op1Index = findRegisters(curInterCodes->code.u.tripleOP.op1, file);
    int op2Index = findRegisters(curInterCodes->code.u.tripleOP.op2, file);
    fprintf(file, "\tmul %s, %s, %s\n", Registers[resultIndex].name, Registers[op1Index].name, Registers[op2Index].name);
    SaveRegister(resultIndex, file);
}

/* 目标代码生成：div */
void generateDivObjectCode(InterCodes curInterCodes, FILE *file)
{
    int resultIndex = findRegisters(curInterCodes->code.u.tripleOP.result, file);
    int op1Index = findRegisters(curInterCodes->code.u.tripleOP.op1, file);
    int op2Index = findRegisters(curInterCodes->code.u.tripleOP.op2, file);
    fprintf(file, "\tdiv %s, %s, %s\n", Registers[resultIndex].name, Registers[op1Index].name, Registers[op2Index].name);
    SaveRegister(resultIndex, file);
}

// 中间代码生成时未产生此下三种类型
void generateGetAddrObjectCode(InterCodes curInterCodes, FILE *file) {}

void generateGetContentObjectCode(InterCodes curInterCodes, FILE *file) {}

void generateToAddrObjectCode(InterCodes curInterCodes, FILE *file) {}

/* 目标代码生成：goto */
void generateGotoObjectCode(InterCodes curInterCodes, FILE *file)
{
    fprintf(file, "\tj label%d\n", curInterCodes->code.u.singleOP.op->var_no);
}

/* 目标代码生成：ifgoto */
void generateIfgotoObjectCode(InterCodes curInterCodes, FILE *file)
{
    int op1Index = findRegisters(curInterCodes->code.u.ifgotoOP.op1, file);
    int op2Index = findRegisters(curInterCodes->code.u.ifgotoOP.op2, file);
    if (strcmp(curInterCodes->code.u.ifgotoOP.relop, "==") == 0)
    {
        fprintf(file, "\tbeq %s, %s, label%d\n", Registers[op1Index].name, Registers[op2Index].name, curInterCodes->code.u.ifgotoOP.label->var_no);
    }
    else if (strcmp(curInterCodes->code.u.ifgotoOP.relop, "!=") == 0)
    {
        fprintf(file, "\tbne %s, %s, label%d\n", Registers[op1Index].name, Registers[op2Index].name, curInterCodes->code.u.ifgotoOP.label->var_no);
    }
    else if (strcmp(curInterCodes->code.u.ifgotoOP.relop, ">") == 0)
    {
        fprintf(file, "\tbgt %s, %s, label%d\n", Registers[op1Index].name, Registers[op2Index].name, curInterCodes->code.u.ifgotoOP.label->var_no);
    }
    else if (strcmp(curInterCodes->code.u.ifgotoOP.relop, "<") == 0)
    {
        fprintf(file, "\tblt %s, %s, label%d\n", Registers[op1Index].name, Registers[op2Index].name, curInterCodes->code.u.ifgotoOP.label->var_no);
    }
    else if (strcmp(curInterCodes->code.u.ifgotoOP.relop, ">=") == 0)
    {
        fprintf(file, "\tbge %s, %s, label%d\n", Registers[op1Index].name, Registers[op2Index].name, curInterCodes->code.u.ifgotoOP.label->var_no);
    }
    else if (strcmp(curInterCodes->code.u.ifgotoOP.relop, "<=") == 0)
    {
        fprintf(file, "\tble %s, %s, label%d\n", Registers[op1Index].name, Registers[op2Index].name, curInterCodes->code.u.ifgotoOP.label->var_no);
    }
    Registers[op1Index].used = 0;
    Registers[op2Index].used = 0;
}

/* 目标代码生成：return */
void generateReturnObjectCode(InterCodes curInterCodes, FILE *file)
{
    fprintf(file, "\tlw $ra, 4($fp)\n");
    fprintf(file, "\taddi $sp, $fp, 8\n");
    int index = findRegisters(curInterCodes->code.u.singleOP.op, file);
    fprintf(file, "\tlw $fp, 0($fp)\n");
    fprintf(file, "\tmove $v0, %s\n", Registers[index].name);
    fprintf(file, "\tjr $ra\n");
    for (int i = 8; i <= 15; i++)
        if (Registers[i].used == 1)
            Registers[i].used = 0;
}

// func中已经处理了dec内容
void generateDecObjectCode(InterCodes curInterCodes, FILE *file) {}

/* 目标代码生成：arg */
void generateArgObjectCode(InterCodes curInterCodes, FILE *file)
{
    int argCount = 0;
    if (curInterCodes->code.kind == ARG_InterCode)
    {
        while (curInterCodes->code.kind == ARG_InterCode)
        {
            argCount++;
            fprintf(file, "\taddi $sp, $sp, -4\n");
            int free = findRegisters(curInterCodes->code.u.singleOP.op, file);
            fprintf(file, "\tsw %s, 0($sp)\n", Registers[free].name);
            curInterCodes = curInterCodes->next;
            Registers[free].used = 0;
        }
    }
    Operand left = curInterCodes->code.u.doubleOP.left;
    Operand right = curInterCodes->code.u.doubleOP.right;
    fprintf(file, "\tjal %s\n", right->funcName);
    fprintf(file, "\taddi $sp, $sp, %d\n", argCount * 4);
    int leftIndex = findRegisters(left, file);
    fprintf(file, "\tmove %s, $v0\n", Registers[leftIndex].name);
    SaveRegister(leftIndex, file);
}

// arg后整体处理
void generateCallObjectCode(InterCodes curInterCodes, FILE *file)
{
}

// func中处理
void generateParamObjectCode(InterCodes curInterCodes, FILE *file)
{
}

/* 目标代码生成：read */
void generateReadObjectCode(InterCodes curInterCodes, FILE *file)
{
    fprintf(file, "\taddi $sp, $sp, -4\n");
    fprintf(file, "\tsw $ra, 0($sp)\n");
    fprintf(file, "\tjal read\n");
    fprintf(file, "\tlw $ra, 0($sp)\n");
    fprintf(file, "\taddi $sp, $sp, 4\n");
    int index = findRegisters(curInterCodes->code.u.singleOP.op, file);
    fprintf(file, "\tmove %s, $v0\n", Registers[index].name);
    SaveRegister(index, file);
}

/* 目标代码生成：write */
void generateWriteObjectCode(InterCodes curInterCodes, FILE *file)
{
    int index = findRegisters(curInterCodes->code.u.singleOP.op, file);
    fprintf(file, "\tmove $a0, %s\n", Registers[index].name);
    fprintf(file, "\taddi $sp, $sp, -4\n");
    fprintf(file, "\tsw $ra, 0($sp)\n");
    fprintf(file, "\tjal write\n");
    fprintf(file, "\tlw $ra, 0($sp)\n");
    fprintf(file, "\taddi $sp, $sp, 4\n");
    for (int i = 8; i <= 15; i++)
        if (Registers[i].used == 1)
            Registers[i].used = 0;
}