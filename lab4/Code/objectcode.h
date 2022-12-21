#include "tools.h"
extern InterCodes interCodeListHead;
typedef struct VarOffset_ *VarOffset;
typedef struct Register
{
    bool used;          // 是否使用
    char *name;         // 寄存器名字
    VarOffset variable; // 所存变量的偏移值：用于save到原地址
} Register;

typedef struct VarOffset_
{
    char name[32]; // 变量名字
    int reg;       // 寄存器号
    int offset;    // 偏移量
    VarOffset next;
} VarOffset_;

/* 初始化所有寄存器 */
void initRegisters();
/* 寻找合适寄存器并填入操作内容 */
int findRegisters(Operand op, FILE *file);
/* 将寄存器值存回原地址 */
void SaveRegister(int index, FILE *file);
/* 获取操作数偏移量 */
VarOffset getVarOffset(Operand op);
/* 创建操作数偏移量 */
void createVarOffset(Operand op);

/* 目标代码生成 */
void generateObjectCode(FILE *file);
void generateLableObjectCode(InterCodes curInterCodes, FILE *file);
void generateFuncObjectCode(InterCodes curInterCodes, FILE *file);
void generateAssignObjectCode(InterCodes curInterCodes, FILE *file);
void generateArithmeticCode(char *opType, InterCodes curInterCodes, FILE *file);
void generateAddObjectCode(InterCodes curInterCodes, FILE *file);
void generateSubObjectCode(InterCodes curInterCodes, FILE *file);
void generateMulObjectCode(InterCodes curInterCodes, FILE *file);
void generateDivObjectCode(InterCodes curInterCodes, FILE *file);
void generateGetAddrObjectCode(InterCodes curInterCodes, FILE *file);
void generateGetContentObjectCode(InterCodes curInterCodes, FILE *file);
void generateToAddrObjectCode(InterCodes curInterCodes, FILE *file);
void generateGotoObjectCode(InterCodes curInterCodes, FILE *file);
void generateIfgotoObjectCode(InterCodes curInterCodes, FILE *file);
void generateReturnObjectCode(InterCodes curInterCodes, FILE *file);
void generateDecObjectCode(InterCodes curInterCodes, FILE *file);
void generateArgObjectCode(InterCodes curInterCodes, FILE *file);
void generateCallObjectCode(InterCodes curInterCodes, FILE *file);
void generateParamObjectCode(InterCodes curInterCodes, FILE *file);
void generateReadObjectCode(InterCodes curInterCodes, FILE *file);
void generateWriteObjectCode(InterCodes curInterCodes, FILE *file);