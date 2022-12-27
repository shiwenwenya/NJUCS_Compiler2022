%{	
	#include <stdio.h>
	#include <stdlib.h>
	#include "tools.h"
	#include "syntax.tab.h"
	extern int errorLexFlag;
	int yycolumn = 1;
	int yylex();
	
%}


%option yylineno

/* digit 数字 */
digit [0-9]
/* letter 字符 */
letter [a-zA-Z]
/* unsignedint 无符号整型 */
unsignedint [1-9]{digit}*

/* ID 标识符 */
ID ({letter}|_)({letter}|_|{digit})*
/* INT 整形 */
INT (0|{unsignedint})
/* OCT int8 */
OCT 0(0|([1-7][0-7]*))
/* HEX int16 */
/*HEX [+-]?(0(x|X))(0|([1-9A-Fa-f][0-9A-Fa-f]*))*/
HEX 0(x|X)[0-9a-fA-F]{1,8}
DOT "."
/* FLOAT 浮点型 */
/*FLOAT {digit}+{DOT}{digit}+|(({digit}+{DOT}{digit}+|{digit}+{DOT}|{DOT}{digit}+)[Ee][+-]?{digit}+)*/
FLOAT ([0-9]+\.[0-9]+)|((([0-9]+\.[0-9]*)|([0-9]*\.[0-9]+))[eE][+-]?[0-9]+)
/* bracket */
LP "(" 
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
SEMI ";"
COMMA ","

/* relop */
RELOP ">="|"<="|"=="|">"|"<"|"!="

/* opt */
ASSIGNOP "="
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
NOT "!"

/* keyword */
STRUCT "struct"
RETURN "return"
IF "if"
ELSE "else"
WHILE "while"
TYPE "int"|"float"

/* other */
WS [ \t\r\n]+
COMMENT "//"

/* error */
OCTERROR 0([0-7]*)?[8-9]+{digit}*
HEXERROR (0(x|X))(0|([1-9A-Fa-f][0-9A-Fa-f]*))?([g-zG-Z]+)({digit}|{letter})*
FLOATERROR 	({digit}+{DOT}{digit}+|{digit}+{DOT}|{DOT}{digit}+)[Ee]
EXCEERROR "= ="|"-"

%%
{WS} {}
{STRUCT} {yylval.node = createNewAbstractSyntaxTreeNode("STRUCT","",2,yylineno);return STRUCT;} 

{FLOAT} {yylval.node = createNewAbstractSyntaxTreeNode("FLOAT",yytext,2,yylineno);return FLOAT;}
{INT} {yylval.node = createNewAbstractSyntaxTreeNode("INT",yytext,2,yylineno);return INT;}
{OCT} {yylval.node = createNewAbstractSyntaxTreeNode("INT",yytext,2,yylineno);return INT;}
{HEX} {yylval.node = createNewAbstractSyntaxTreeNode("INT",yytext,2,yylineno);return INT;}

{TYPE} {yylval.node = createNewAbstractSyntaxTreeNode("TYPE",yytext,2,yylineno);return TYPE;}
{RETURN} {yylval.node = createNewAbstractSyntaxTreeNode("RETURN","",2,yylineno);return RETURN;}
{IF} {yylval.node = createNewAbstractSyntaxTreeNode("IF","",2,yylineno);return IF;}
{ELSE} {yylval.node = createNewAbstractSyntaxTreeNode("ELSE","",2,yylineno);return ELSE;}
{WHILE} {yylval.node = createNewAbstractSyntaxTreeNode("WHILE","",2,yylineno);return WHILE;}

{ID} {yylval.node = createNewAbstractSyntaxTreeNode("ID",yytext,2,yylineno);return ID;}

{LP} {yylval.node = createNewAbstractSyntaxTreeNode("LP","",2,yylineno);return LP;} 
{RP} {yylval.node = createNewAbstractSyntaxTreeNode("RP","",2,yylineno);return RP;}
{LB} {yylval.node = createNewAbstractSyntaxTreeNode("LB","",2,yylineno);return LB;}
{RB} {yylval.node = createNewAbstractSyntaxTreeNode("RB","",2,yylineno);return RB;}
{LC} {yylval.node = createNewAbstractSyntaxTreeNode("LC","",2,yylineno);return LC;}
{RC} {yylval.node = createNewAbstractSyntaxTreeNode("RC","",2,yylineno);return RC;}
{SEMI} {yylval.node = createNewAbstractSyntaxTreeNode("SEMI","",2,yylineno);return SEMI;}
{RELOP} {yylval.node = createNewAbstractSyntaxTreeNode("RELOP",yytext,2,yylineno);return RELOP;} 
{COMMA} {yylval.node = createNewAbstractSyntaxTreeNode("COMMA","",2,yylineno);return COMMA;}
{ASSIGNOP} {yylval.node = createNewAbstractSyntaxTreeNode("ASSIGNOP","",2,yylineno);return ASSIGNOP;}
{PLUS} {yylval.node = createNewAbstractSyntaxTreeNode("PLUS","",2,yylineno);return PLUS;}
{MINUS} {yylval.node = createNewAbstractSyntaxTreeNode("MINUS","",2,yylineno);return MINUS;}
{STAR} {yylval.node = createNewAbstractSyntaxTreeNode("STAR","",2,yylineno);return STAR;}
{DIV} {yylval.node = createNewAbstractSyntaxTreeNode("DIV","",2,yylineno);return DIV;}
{AND} {yylval.node = createNewAbstractSyntaxTreeNode("AND","",2,yylineno);return AND;}
{OR} {yylval.node = createNewAbstractSyntaxTreeNode("OR","",2,yylineno);return OR;}
{NOT} {yylval.node = createNewAbstractSyntaxTreeNode("NOT","",2,yylineno);return NOT;}
{DOT} {yylval.node = createNewAbstractSyntaxTreeNode("DOT","",2,yylineno);return DOT;}

{COMMENT} {char temp = input();while(temp!='\n')  temp = input();}
"/*" {
    char c;
    int currentlineno = yylineno;
    while ('\0' != (c = input())) {
        if (c == '*') {
            while ('*' == (c = input()));
            // printf("%c ", c);
            if (c == '/') {
                break;
            }
        }
    }
    if (c == '\0') {
printf("Error type B at Line %d: Syntax error.\n",yylineno);errorLexFlag=1;    }
}

{OCTERROR} {printf("Error type A at Line %d: Illegal octal number \'%s\'\n",yylineno, yytext);errorLexFlag=1;}
{HEXERROR} {printf("Error type A at Line %d: Illegal hexadecimal number \'%s\'\n",yylineno, yytext);errorLexFlag=1;}
. {printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno, yytext);errorLexFlag=1;}


%%