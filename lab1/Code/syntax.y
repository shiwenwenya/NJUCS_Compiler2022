%{ 
	#include "tools.h"
	#include "lex.yy.c"
	extern int errorSyntaxFlag;
	extern int errorLexFlag;
	extern int yylex();
	int yyerror(char* msg);
	extern int yylineno;
%}

%union {
    int type_int;
    float type_float;
    char* type_string;
    AST* node;
};



/*--------------------High-level Definitions--------------------*/
/* tokens defination */
%token <node> INT FLOAT
%token <node> LP RP LB RB LC RC SEMI COMMA
%token <node> RELOP
%token <node> ASSIGNOP PLUS MINUS STAR DIV AND OR DOT NOT
%token <node> STRUCT RETURN IF ELSE WHILE TYPE error
%token <node> ID

%right ASSIGNOP
%left OR
%left AND
%left RELOP 
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier
%type <node> OptTag Tag VarDec FunDec VarList ParamDec CompSt
%type <node> StmtList Stmt DefList Def DecList Dec Exp Args

%%

/*--------------------High-level Definitions--------------------*/
Program : ExtDefList {
        $$=createNewAbstractSyntaxTreeNode("Program","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$1);
        root=$$;
    }
;

ExtDefList : ExtDef ExtDefList {
        $$=createNewAbstractSyntaxTreeNode("ExtDefList","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
    }
|	/*empty*/ {
        $$ = NULL;
    }
;

ExtDef : Specifier ExtDecList SEMI {
	    $$ = createNewAbstractSyntaxTreeNode("ExtDef","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
    }
| Specifier SEMI {
	    $$ = createNewAbstractSyntaxTreeNode("ExtDef","",0,$1->lineNumber);
	    addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| Specifier FunDec CompSt {
	    $$ = createNewAbstractSyntaxTreeNode("ExtDef","",0,$1->lineNumber);
	    addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
|Specifier error SEMI {
	errorSyntaxFlag++;
}
;

ExtDecList : VarDec {
	    $$ = createNewAbstractSyntaxTreeNode("ExtDecList","",0,$1->lineNumber);
	    addAbstractSyntaxTreeNode($$,$1);
	}
| VarDec COMMA ExtDecList {
	    $$ = createNewAbstractSyntaxTreeNode("ExtDecList","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
	    addAbstractSyntaxTreeNode($$,$1);
	}
;

Specifier : TYPE  {
	    $$ = createNewAbstractSyntaxTreeNode("Specifier","",0,$1->lineNumber);
	    addAbstractSyntaxTreeNode($$,$1);
	}
| StructSpecifier  {
	    $$ = createNewAbstractSyntaxTreeNode("Specifier","",0,$1->lineNumber);
	    addAbstractSyntaxTreeNode($$,$1);
	}
;

StructSpecifier : STRUCT OptTag LC DefList RC {
	    $$ = createNewAbstractSyntaxTreeNode("StructSpecifier","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$5);
        addAbstractSyntaxTreeNode($$,$4);
	    addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| STRUCT Tag {
	    $$ = createNewAbstractSyntaxTreeNode("StructSpecifier","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| error{
	errorSyntaxFlag++;
}
;

OptTag : ID {
	    $$ = createNewAbstractSyntaxTreeNode("OptTag","",0,$1->lineNumber);
	    addAbstractSyntaxTreeNode($$,$1);
	}
|   /* empty */  {
	    $$ = NULL;
	}
;

Tag : ID {
	    $$ = createNewAbstractSyntaxTreeNode("Tag","",0,$1->lineNumber);
	    addAbstractSyntaxTreeNode($$,$1);
	}
;

VarDec : ID {
	    $$ = createNewAbstractSyntaxTreeNode("VarDec","",0,$1->lineNumber);
	    addAbstractSyntaxTreeNode($$,$1);
	}
| VarDec LB INT RB {
	    $$ = createNewAbstractSyntaxTreeNode("VarDec","",0,$1->lineNumber);
	    addAbstractSyntaxTreeNode($$,$4);
	    addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
;

FunDec : ID LP VarList RP {
	    $$ = createNewAbstractSyntaxTreeNode("FunDec","",0,$1->lineNumber);
	    addAbstractSyntaxTreeNode($$,$4);
	    addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| ID LP RP {
	    $$ = createNewAbstractSyntaxTreeNode("FunDec","",0,$1->lineNumber);
	    addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| ID LP error RP {
		errorSyntaxFlag++;
	}
| error LP VarList RP{
        errorSyntaxFlag++;
    };
;

VarList : ParamDec COMMA VarList {
		$$ = createNewAbstractSyntaxTreeNode("VarList","",0,$1->lineNumber);
	    addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| ParamDec {
		$$ = createNewAbstractSyntaxTreeNode("VarList","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$1);
	}
;

ParamDec : Specifier VarDec {
		$$ = createNewAbstractSyntaxTreeNode("ParamDec","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$2);
		addAbstractSyntaxTreeNode($$,$1);
	}
;

CompSt : LC DefList StmtList RC {
		$$ = createNewAbstractSyntaxTreeNode("CompSt","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$4);
	    addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| error RC {
		errorSyntaxFlag++;
	}
;

StmtList : Stmt StmtList {
		$$ = createNewAbstractSyntaxTreeNode("StmtList","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| /* empty */ {
		$$ = NULL;
	}
;

Stmt : Exp SEMI {
		$$ = createNewAbstractSyntaxTreeNode("Stmt","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| CompSt {
		$$ = createNewAbstractSyntaxTreeNode("Stmt","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$1);
	}
| RETURN Exp SEMI {
		$$ = createNewAbstractSyntaxTreeNode("Stmt","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {
		$$ = createNewAbstractSyntaxTreeNode("Stmt","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$5);
        addAbstractSyntaxTreeNode($$,$4);
        addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| IF LP Exp RP Stmt ELSE Stmt {
		$$ = createNewAbstractSyntaxTreeNode("Stmt","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$7);
        addAbstractSyntaxTreeNode($$,$6);
        addAbstractSyntaxTreeNode($$,$5);
        addAbstractSyntaxTreeNode($$,$4);
        addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| WHILE LP Exp RP Stmt {
		$$ = createNewAbstractSyntaxTreeNode("Stmt","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$5);
        addAbstractSyntaxTreeNode($$,$4);
        addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| error SEMI {
		errorSyntaxFlag++;
	}

;

DefList : Def DefList {
		$$ = createNewAbstractSyntaxTreeNode("DefList","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| /* empty */  {
		$$ = NULL;
	}
;

Def : Specifier DecList SEMI {
		$$ = createNewAbstractSyntaxTreeNode("Def","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| Specifier error SEMI {
		errorSyntaxFlag++;
	}
;

DecList : Dec {
		$$ = createNewAbstractSyntaxTreeNode("DecList","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$1);
	}
| Dec COMMA DecList {
		$$ = createNewAbstractSyntaxTreeNode("DecList","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
;

Dec : VarDec {
		$$ = createNewAbstractSyntaxTreeNode("Dec","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$1);
	}
| VarDec ASSIGNOP Exp {
		$$ = createNewAbstractSyntaxTreeNode("Dec","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
;

Exp : Exp ASSIGNOP Exp {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| Exp AND Exp {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| Exp OR Exp {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| Exp RELOP Exp {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| Exp PLUS Exp {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| Exp MINUS Exp {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| Exp STAR Exp {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| Exp DIV Exp {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| LP Exp RP {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| MINUS Exp {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| NOT Exp {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| ID LP Args RP {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$4);
		addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| ID LP RP {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| Exp LB Exp RB {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$4);
		addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| Exp DOT ID {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| ID {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$1);
	}
| INT {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
        addAbstractSyntaxTreeNode($$,$1);
	}
| FLOAT {
		$$ = createNewAbstractSyntaxTreeNode("Exp","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$1);
	}
| error{
	errorSyntaxFlag++;
}
;

Args : Exp COMMA Args {
		$$ = createNewAbstractSyntaxTreeNode("Args","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$3);
        addAbstractSyntaxTreeNode($$,$2);
        addAbstractSyntaxTreeNode($$,$1);
	}
| Exp {
		$$ = createNewAbstractSyntaxTreeNode("Args","",0,$1->lineNumber);
		addAbstractSyntaxTreeNode($$,$1);
	}
;

%%

int yyerror(char*msg){
	if(!errorLexFlag){
		    printf("Error type B at Line %d: %s near %s.\n",yylineno,msg,yytext);
	}
}




