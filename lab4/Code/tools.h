#ifndef TOOLS_H
#define TOOLS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#define bool int
#define TRUE 1
#define FALSE 0
#define TABLESIZE 0x3fff

typedef struct HashTableNode_ *HashTableNode;
typedef struct Type_ *Type;
typedef struct FieldList_ *FieldList;
typedef struct SymbolTableNode_ *SymbolTableNode;
typedef struct FunctionTable_ *FunctionTable;
typedef struct Operand_ *Operand;
typedef struct InterCodes_ *InterCodes;

/* AbstractSyntaxTree 抽象语法树 */
typedef struct AbstractSyntaxTree
{
    int lineNumber;                               // 行数
    int abstractSyntaxTreeNodeType;               // 节点类型
    char *name;                                   // 节点名称
    char *value;                                  // 节点值
    struct AbstractSyntaxTree *firstChildrenNode; // 第一个孩子节点
    struct AbstractSyntaxTree *nextSiblingNode;   // 下一个兄弟节点
} AST;

/* Type_ 节点信息 */
typedef struct Type_
{
    enum
    {
        BASIC,
        ARRAY,
        STRUCTURE,
        FUNCTION
    } kind;
    union
    {
        int basic; //基本类型:0为int, 1为float
        struct
        {
            int size;     //数组大小
            Type element; //元素类型
        } array;
        struct
        { //数组类型信息
            char *name;
            FieldList structures;
        } structure; //结构体类型信息
        struct
        {
            int parameterNum; //参数个数
            FieldList parameters;
            Type returnType; //返回值类型
        } function;          //函数类型信息
    } u;
} Type_;

/* FieldList_ 域信息 */
typedef struct FieldList_
{
    char *name;              //域的名字
    Type type;               //域的类型
    FieldList nextFieldList; //下一个域
} FieldList_;

/* SymbolTableNode_ 符号表节点 */
typedef struct SymbolTableNode_
{
    Type type;
    char *name;
    int kind;       // 0 var 1 struct 2 function
    bool isDefined; //是否定义
    int depth;
    int var_no;
    int isAddress;
    int offset;
    char *structName;
    SymbolTableNode sameHashSymbolTableNode;
    SymbolTableNode controlScopeSymbolTableNode;
} SymbolTableNode_;

/* HashTable 符号表 */
typedef struct HashTableNode_
{
    SymbolTableNode symbolTableNode;
    HashTableNode nextHashTableNode;
} HashTableNode_;

/* FunctionTable 函数表 */
struct FunctionTable_
{
    char *name;
    int functionLineNumber;
    FunctionTable next;
};

/* SemanticError 错误类型 */
typedef enum SemanticError
{
    Undefined_Variable,
    Undefined_Function,
    Redefined_Variable_Name,
    Redefined_Function,
    AssignOP_Type_Dismatch,
    Leftside_Rvalue_Error,
    Operand_Type_Dismatch,
    Return_Type_Dismatch,
    Func_Call_Parameter_Dismatch,
    Operate_Others_As_Array,
    Operate_Basic_As_Func,
    Array_Float_Index,
    Operate_Others_As_Struct,
    Undefined_Field,
    Redefined_Field,
    Redefined_Field_Name,
    Undefined_Struct,
    Undefined_Function_But_Declaration,
    Conflict_Decordef_Funcion
} SemanticError;

struct Operand_
{
    enum
    {
        VARIABLE_OP, //变量
        CONSTANT_OP, //常量
        TEMP_OP,     //临时变量
        FUNCTION_OP, //函数
        LABEL_OP     //标号
    } kind;
    enum
    {
        VAL,
        ADDRESS
    } type;
    int var_no;     //标号数
    int value;      //数值
    char *varName;  //变量名
    char *funcName; //函数名
    int depth;
} Operand_;

struct InterCode
{
    enum
    {
        LABEL_InterCode,       //定义标号x LABEL x :
        FUNC_InterCode,        //定义函数f FUNCTION f :
        ASSIGN_InterCode,      //赋值操作 x := y
        ADD_InterCode,         //加法操作 x := y + z
        SUB_InterCode,         //减法操作 x := y - z
        MUL_InterCode,         //乘法操作 x := y * z
        DIV_InterCode,         //除法操作 x := y / z
        GET_ADDR_InterCode,    //取y的地址赋给x x := &y
        GET_CONTENT_InterCode, //取以y值为地址的内存单元的内容赋给x x := *y
        TO_ADDR_InterCode,     //取y值赋给以x值为地址的内存单元 *x := y
        GOTO_InterCode,        //无条件跳转至标号x GOTO x
        IFGOTO_InterCode,      //如果x与y满足[relop]关系则跳转至标号z IF x [relop] y GOTO z
        RETURN_InterCode,      //退出当前函数并返回x值 RETURN x
        DEC_InterCode,         //内存空间申请，大小为4的倍数 DEC x [size]
        ARG_InterCode,         //传实参x ARG x
        CALL_InterCode,        //调用函数，并将其返回值赋给x x := CALL f
        PARAM_InterCode,       //函数参数声明 PARAM x
        READ_InterCode,        //从控制台读取x的值 READ x
        WRITE_InterCode        //向控制台打印x的值 WRITE x
    } kind;
    union
    {
        struct
        {
            Operand op;
        } singleOP; // LABEL FUNC GOTO RETURN ARG PARAM READ WRITE
        struct
        {
            Operand left, right;
        } doubleOP; // ASSIGN GET_ADDR GET_CONTENT TO_ADDR CALL DEC
        struct
        {
            Operand result, op1, op2;
        } tripleOP; // ADD SUB MUL DIV
        struct
        {
            Operand op1, op2, label;
            char *relop;
        } ifgotoOP; // IFGOTO
    } u;
} InterCode;

struct InterCodes_
{
    struct InterCode code;
    InterCodes prev, next;
} InterCodes_; //双向链表

extern AST *root;
extern int yylineno;
extern int varNo;
extern int tempNo;
extern int labelNo;
extern InterCodes interCodeListHead;
extern InterCodes interCodeListTail;

/* createNewAbstractSyntaxTreeNode 创建新语法树节点 */
AST *createNewAbstractSyntaxTreeNode(char *name, char *value, int type, int lineNumber);
/* addAbstractSyntaxTreeNode 添加语法树节点 */
void addAbstractSyntaxTreeNode(AST *pre, AST *next);
/* deleteAbstractSyntaxTree 删除语法树 */
void deleteAbstractSyntaxTree(AST *root);
/* printAbstractSyntaxTree 打印抽象语法树 */
void printAbstractSyntaxTree(AST *root, int depth);
/* printSpaces 打印空格 */
void printSpaces(int count);
/* stringComparison 字符串比较 */
bool stringComparison(char *src, char *dst);
/* printNodeMessage 打印信息 */
void printNodeMessage(char *name, char *value);
/* to_Oct 转换成八进制数 */
int to_Oct(char *str);
/* to_Hex 转换成十六进制数 */
int to_Hex(char *str);
/* to_Dec 转换成十进制数 */
int to_Dec(char *str);
/* My_atoi 自定义atoi */
int My_atoi(char *str);
char *My_itoa(int num, char *str);
/* hash_pjw 散列函数 */
unsigned int hash_pjw(char *name);
/* initHashTable 初始化符号表 */
HashTableNode initHashTable();
/* initSymbolTableNode 初始化符号表节点 */
SymbolTableNode initSymbolTableNode(Type type, char *name, int isDefined, int depth);
/* getSymbolTableNode 查找符号表节点 */
SymbolTableNode getSymbolTableNode(char *name, int depth);
/* insertHashTable 节点插入符号表 */
void insertHashTable(SymbolTableNode symbolTableNode, HashTableNode hashTableNode);
/* enterInnermostHashTable 进入哈希表最内层 */
HashTableNode enterInnermostHashTable();
/* deleteLocalVariable 删除局部变量 */
void deleteLocalVariable();
/* 删除节点 */
void deleteNode(char *name, int depth, HashTableNode hashTableNode);
/* createNewSymbolTableNode 创建新符号表节点 */
SymbolTableNode createNewSymbolTableNode(Type type, char *name, int kind, bool isDefined, int depth);
/* insertFunction 插入函数节点 */
void insertFunction(char *name, int functionLineNumber);
/* insertStruct 插入结构体节点 */
int insertStruct(Type type, char *name, int offset, char *structName);
/* getLocalVariable 寻找局部变量 */
bool getLocalVariable(Type *type, char *name, int *isDefined, int depth, int mkd);
/* getGlobalVariable 寻找全局变量 */
bool getGlobalVariable(Type *type, char *name, int *isDefined, int depth, int *kind);
/* getStruct 查找结构体节点 */
SymbolTableNode getStruct(char *name);
/* isExistStruct查找结构体 */
bool isExistStruct(Type *type, char *name);
/* printSemanticError 打印语义错误 */
void printSemanticError(enum SemanticError errortype, int line, char *message);
/* typeEqual 检查两个类型是否相等 */
bool typeEqual(Type t1, Type t2);
/* arrayStrongEqual 检查两个数组是否强相等 */
bool arrayStrongEqual(Type t1, Type t2);
/* getChild 获取子节点 */
AST *getChild(AST *root, int childnum);
/* CheckFunc 检查未定义函数*/
void CheckFunc();

/* insertInterTableList 双向链表插入结点 */
void insertInterCodesList(InterCodes curInterCode);
/* createInterCodes 创建双向链表成员 */
void createInterCodes(int kind, ...);
/* createOperand 创建符号 */
Operand createOperand(int kind, int type, ...);
/* printOperand 打印符号 */
void printOperand(Operand op, FILE *file);
/* printInterCodes 打印中间代码 */
void printInterCodes(FILE *file);
/* copyOperand 复制符号 */
Operand copyOperand(Operand operand);
/* NegateRelop 判断符号取反 */
char *NegateRelop(char *relop);
/* 获取类型大小 */
int getTypeSize(Type type);
#endif