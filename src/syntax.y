%{
    #include "tokentree.h"

    #include "lex.yy.c"
    void yyerror(const char*);

    #define YYSTYPE struct node *

    // int yydebug=1;
    struct node *root;

    int error_flag=0;
%}
%token INT CHAR FLOAT ID
%token TYPE
%token STRUCT IF WHILE RETURN
%token SEMI COMMA 
%token LC RC
%token ERRORA

%precedence LOWER_IF
%precedence ELSE


%right ASSIGN
%left OR
%left AND
%left LT LE GT GE NE EQ
%left PLUS MINUS
%left MUL DIV
%right UMINUS NOT
%left DOT LP RP LB RB

%%
/* high-level definition */
Program: ExtDefList {$$=new_node("Program","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1); root=$$;}
    ;
ExtDefList: ExtDef ExtDefList {$$=new_node("ExtDefList","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}
    | %empty {$$=new_node("ExtDefList","",-1,NONTERMINAL);}
    ;
ExtDef: Specifier ExtDecList SEMI {$$=new_node("ExtDef","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Specifier SEMI {$$=new_node("ExtDef","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}
    | Specifier FunDec CompSt {$$=new_node("ExtDef","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    ;
ExtDecList: VarDec {$$=new_node("ExtDecList","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | VarDec COMMA ExtDecList {$$=new_node("ExtDecList","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    ;

/* specifier */
Specifier: TYPE {$$=new_node("Specifier","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | StructSpecifier {$$=new_node("Specifier","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    ;
StructSpecifier: STRUCT ID LC DefList RC {$$=new_node("StructSpecifier","",$1->lineno,NONTERMINAL); link_nodes($$,5,$1,$2,$3,$4,$5);}
    | STRUCT ID {$$=new_node("StructSpecifier","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}
    ;

/* declarator */
VarDec: ID {$$=new_node("VarDec","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | VarDec LB INT RB {$$=new_node("VarDec","",$1->lineno,NONTERMINAL); link_nodes($$,4,$1,$2,$3,$4);}
    ;
FunDec: ID LP VarList RP {$$=new_node("FunDec","",$1->lineno,NONTERMINAL); link_nodes($$,4,$1,$2,$3,$4);}
    | ID LP RP {$$=new_node("FunDec","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | ID LP error {printf("Error type B at Line %d: Missing closing parenthesis ')'\n",$2->lineno);}
    ;
VarList: ParamDec COMMA VarList {$$=new_node("VarList","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | ParamDec {$$=new_node("VarList","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    ;
ParamDec: Specifier VarDec {$$=new_node("ParamDec","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}
    ;

/* statement */
CompSt: LC DefList StmtList RC {$$=new_node("CompSt","",$1->lineno,NONTERMINAL); link_nodes($$,4,$1,$2,$3,$4);}
    ;
StmtList: Stmt StmtList {$$=new_node("StmtList","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}
    | %empty {$$=new_node("StmtList","",-1,NONTERMINAL);}
    ;
Stmt: Exp SEMI {$$=new_node("Stmt","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}
    | Exp error {printf("Error type B at Line %d: Missing semicolon ';'\n",$1->lineno);}
    | CompSt {$$=new_node("Stmt","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | RETURN Exp SEMI {$$=new_node("Stmt","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | RETURN Exp error {printf("Error type B at Line %d: Missing semicolon ';'\n",$2->lineno);}
    | IF LP Exp RP Stmt %prec LOWER_IF {$$=new_node("Stmt","",$1->lineno,NONTERMINAL); link_nodes($$,5,$1,$2,$3,$4,$5);}
    | IF LP Exp RP Stmt ELSE Stmt {$$=new_node("Stmt","",$1->lineno,NONTERMINAL); link_nodes($$,7,$1,$2,$3,$4,$5,$6,$7);}
    | WHILE LP Exp RP Stmt {$$=new_node("Stmt","",$1->lineno,NONTERMINAL); link_nodes($$,5,$1,$2,$3,$4,$5);}
    ;

/* local definition */
DefList: Def DefList {$$=new_node("DefList","",2,NONTERMINAL); link_nodes($$,2,$1,$2);}
    | %empty {$$=new_node("DefList","",-1,NONTERMINAL);}
    ;
Def: Specifier DecList SEMI {$$=new_node("Def","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    |Specifier DecList error {printf("Error type B at Line %d: Missing semicolon ';'\n",$2->lineno);}
    ;
DecList: Dec {$$=new_node("DecList","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | Dec COMMA DecList {$$=new_node("DecList","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    ;
Dec: VarDec {$$=new_node("Dec","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | VarDec ASSIGN Exp {$$=new_node("Dec","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    ;

/* Expression */
Exp: Exp ASSIGN Exp {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Exp AND Exp {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Exp OR Exp {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Exp LT Exp {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Exp LE Exp {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Exp GT Exp {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Exp GE Exp {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Exp NE Exp {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Exp EQ Exp {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Exp PLUS Exp {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Exp MINUS Exp {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Exp MUL Exp {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Exp DIV Exp {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | LP Exp RP {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | MINUS Exp %prec UMINUS {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}
    | NOT Exp {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}
    | ID LP Args RP {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,4,$1,$2,$3,$4);}
    | ID LP RP {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Exp LB Exp RB {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,4,$1,$2,$3,$4);}
    | Exp DOT ID {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | ID {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | INT {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | FLOAT {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | CHAR {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | ID LP error {printf("Error type B at Line %d: Missing closing parenthesis ')'\n",$2->lineno);}
    ;
Args: Exp COMMA Args {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Exp {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}

%%
void yyerror(const char *s) {
    // fprintf(stderr, "%s\n", s);
    error_flag=1;
}
int main() {
    int val=yyparse();
    if (error_flag==0) {
        print_tree(root,0);
    }
}
