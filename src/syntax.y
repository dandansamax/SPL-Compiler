%{
    #include "tokentree.h"
    #include "preprocess.h"
    #include <unistd.h>

    #include "lex.yy.c"
    void yyerror(const char*);

    #define YYSTYPE struct node *

    // int yydebug=1;
    struct node *root;

    #define MISSING_RP(loc) fprintf(output_file,"Error type B at Line %d: Missing closing parenthesis ')'\n",loc->lineno);
    #define MISSING_LP(loc) fprintf(output_file,"Error type B at Line %d: Missing closing parenthesis '('\n",loc->lineno);
    #define MISSING_LP_RP(loc) fprintf(output_file,"Error type B at Line %d: Missing closing parenthesis '(' and ')'\n",loc->lineno);
    #define MISSING_LB(loc) fprintf(output_file,"Error type B at Line %d: Missing closing parenthesis '['\n",loc->lineno);
    #define MISSING_RB(loc) fprintf(output_file,"Error type B at Line %d: Missing closing parenthesis ']'\n",loc->lineno);
    #define MISSING_LB_RB(loc) fprintf(output_file,"Error type B at Line %d: Missing closing parenthesis '[' and ']'\n",loc->lineno);
    #define MISSING_LC(loc) fprintf(output_file,"Error type B at Line %d: Missing closing parenthesis '{'\n",loc->lineno);
    #define MISSING_RC(loc) fprintf(output_file,"Error type B at Line %d: Missing closing parenthesis '}'\n",loc->lineno);
    #define MISSING_LC_RC(loc) fprintf(output_file,"Error type B at Line %d: Missing closing parenthesis '{' and '}'\n",loc->lineno);
    #define REDUNDANT_SEMI(loc) fprintf(output_file,"Error type B at Line %d: Redundant semicolon\n",loc->lineno);
    #define REDUNDANT_TYPE(loc) fprintf(output_file,"Error type B at Line %d: Redundant type\n",loc->lineno);
    #define MISSING_DECLARATION_CONTENT(loc) fprintf(output_file,"Error type B at Line %d: Missing declaration content\n",loc->lineno);
    #define MISSING_EXP(loc,c) fprintf(output_file,"Error type B at Line %d: Missing expresion after '"#c"'\n",loc->lineno);
    #define MISSING_SEMI(loc) fprintf(output_file,"Error type B at Line %d: Missing semicolon ';'\n",loc->lineno);
    #define MISPLACE_DEF(loc) fprintf(output_file,"Error type B at Line %d: Misplaced defination\n",loc->lineno);
    #define MISPLACE_ARR(loc) fprintf(output_file,"Error type B at Line %d: Invalid array declaration\n",loc->lineno);
    #define INVALID_FOR(loc) fprintf(output_file,"Error type B at Line %d: Invalid 'for' statement \n",loc->lineno);
%}
%define parse.error verbose

%token INT CHAR FLOAT ID
%token TYPE
%token STRUCT IF WHILE RETURN FOR
%token SEMI COMMA 
%token LC RC

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

%left UNKNOWN

%%
/* high-level definition */
Program: ExtDefList {$$=new_node("Program","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1); root=$$;}
    ;
ExtDefList: ExtDef ExtDefList {$$=new_node("ExtDefList","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}
    | %empty {$$=new_node("ExtDefList","",-1,NONTERMINAL);}
    ;
ExtDef: Specifier ExtDecList SEMI {$$=new_node("ExtDef","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Specifier ExtDecList SEMI SEMI{REDUNDANT_SEMI($3)}
    | Specifier SEMI {$$=new_node("ExtDef","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}
    | Specifier SEMI SEMI {REDUNDANT_SEMI($2)}
    | Specifier FunDec CompSt {$$=new_node("ExtDef","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    ;
ExtDecList: VarDec {$$=new_node("ExtDecList","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | VarDec COMMA ExtDecList {$$=new_node("ExtDecList","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    ;

/* specifier */
Specifier: 
    TYPE TYPE error {REDUNDANT_TYPE($1)}
    | TYPE LB INT RB error{MISPLACE_ARR($1)}
    | TYPE LB RB error{MISPLACE_ARR($1)}
    | TYPE {$$=new_node("Specifier","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | StructSpecifier {$$=new_node("Specifier","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    ;
StructSpecifier: STRUCT ID LC DefList RC {$$=new_node("StructSpecifier","",$1->lineno,NONTERMINAL); link_nodes($$,5,$1,$2,$3,$4,$5);}
    | STRUCT ID LC DefList error {MISSING_RC($3)}
    | STRUCT ID error DefList RC {MISSING_LC($2)}
    | STRUCT ID error DefList error {MISSING_LC_RC($2)}
    | STRUCT ID {$$=new_node("StructSpecifier","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}
    | STRUCT STRUCT ID LC DefList RC error {REDUNDANT_TYPE($1)}
    | STRUCT STRUCT ID{REDUNDANT_TYPE($1)}
    ;

/* declarator */
VarDec: ID {$$=new_node("VarDec","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | VarDec LB INT RB {$$=new_node("VarDec","",$1->lineno,NONTERMINAL); link_nodes($$,4,$1,$2,$3,$4);}
    | VarDec LB INT error {MISSING_RB($3)}
    | VarDec INT RB error {MISSING_LB($3)}
    | VarDec INT error {MISSING_LB_RB($3)}

    ;
FunDec: ID LP VarList RP {$$=new_node("FunDec","",$1->lineno,NONTERMINAL); link_nodes($$,4,$1,$2,$3,$4);}
    | ID VarList RP {MISSING_LP($2)}
    | ID LP RP {$$=new_node("FunDec","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | ID LP error {MISSING_RP($2)}
    | ID error RP {MISSING_LP($2)}
    | ID VarList error{MISSING_LP_RP($1)}
    | ID error{MISSING_LP_RP($1)}
    ;
VarList: ParamDec COMMA VarList {$$=new_node("VarList","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | ParamDec {$$=new_node("VarList","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    ;
ParamDec: Specifier VarDec {$$=new_node("ParamDec","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}
    ;

/* statement */
CompSt: LC DefList StmtList RC {$$=new_node("CompSt","",$1->lineno,NONTERMINAL); link_nodes($$,4,$1,$2,$3,$4);}
    | LC DefList StmtList error{MISSING_RC($3)}
    ;
StmtList: Stmt StmtList {$$=new_node("StmtList","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}
    | %empty {$$=new_node("StmtList","",-1,NONTERMINAL);}
    | Stmt Def StmtList error {MISPLACE_DEF($2)}
    ;
Stmt: Exp SEMI {$$=new_node("Stmt","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}
    | Exp SEMI SEMI error {REDUNDANT_SEMI($2)}
    | Exp error {MISSING_SEMI($1)}
    | CompSt {$$=new_node("Stmt","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | RETURN Exp SEMI {$$=new_node("Stmt","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | RETURN Exp SEMI SEMI error {REDUNDANT_SEMI($3)}
    | RETURN Exp error {MISSING_SEMI($2)}
    | IF LP Exp RP Stmt %prec LOWER_IF {$$=new_node("Stmt","",$1->lineno,NONTERMINAL); link_nodes($$,5,$1,$2,$3,$4,$5);}
    | IF LP Exp RP Stmt ELSE Stmt {$$=new_node("Stmt","",$1->lineno,NONTERMINAL); link_nodes($$,7,$1,$2,$3,$4,$5,$6,$7);}
    | IF error Exp RP Stmt %prec LOWER_IF {MISSING_LP($1)}
    | IF error Exp RP Stmt ELSE Stmt {MISSING_LP($1)}
    | IF error Exp error Stmt %prec LOWER_IF {MISSING_LP_RP($1)}
    | IF error Exp error Stmt ELSE Stmt {MISSING_LP_RP($1)}
    | WHILE LP Exp RP Stmt {$$=new_node("Stmt","",$1->lineno,NONTERMINAL); link_nodes($$,5,$1,$2,$3,$4,$5);}
    | WHILE error Exp RP Stmt {MISSING_LP($1)}
    | WHILE error Exp error Stmt {MISSING_LP_RP($1)}
    | FOR LP ForArgs RP Stmt {$$=new_node("Stmt","",$1->lineno,NONTERMINAL); link_nodes($$,5,$1,$2,$3,$4,$5);}
    | FOR LP error RP Stmt {INVALID_FOR($2)}
    | FOR error ForArgs RP Stmt {MISSING_LP($1)}
    | FOR LP ForArgs error Stmt {MISSING_RP($2)}
    ;

ForArgs: Exp SEMI Exp SEMI Exp {$$=new_node("ForArgs","",$1->lineno,NONTERMINAL); link_nodes($$,5,$1,$2,$3,$4,$5);}
    | SEMI Exp SEMI Exp {$$=new_node("ForArgs","",$1->lineno,NONTERMINAL); link_nodes($$,4,$1,$2,$3,$4);}
    | Exp SEMI SEMI Exp {$$=new_node("ForArgs","",$1->lineno,NONTERMINAL); link_nodes($$,4,$1,$2,$3,$4);}
    | Exp SEMI Exp SEMI {$$=new_node("ForArgs","",$1->lineno,NONTERMINAL); link_nodes($$,4,$1,$2,$3,$4);}
    | SEMI SEMI Exp {$$=new_node("ForArgs","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | SEMI Exp SEMI {$$=new_node("ForArgs","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Exp SEMI SEMI {$$=new_node("ForArgs","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | SEMI SEMI {$$=new_node("ForArgs","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}

/* local definition */
DefList: Def DefList {$$=new_node("DefList","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}
    | %empty {$$=new_node("DefList","",-1,NONTERMINAL);}
    ;
Def: Specifier DecList SEMI {$$=new_node("Def","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    |Specifier DecList SEMI SEMI {REDUNDANT_SEMI($3)}
    |Specifier DecList error {MISSING_SEMI($2)}
    |Specifier SEMI error{MISSING_DECLARATION_CONTENT($1)}
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
    | Exp ASSIGN error {MISSING_EXP($2,=)}
    | Exp AND error {MISSING_EXP($2,&&)}
    | Exp OR error {MISSING_EXP($2,||)}
    | Exp LT error {MISSING_EXP($2,<)}
    | Exp LE error {MISSING_EXP($2,<=)}
    | Exp GT error {MISSING_EXP($2,>)}
    | Exp GE error {MISSING_EXP($2,>=)}
    | Exp NE error {MISSING_EXP($2,!=)}
    | Exp EQ error {MISSING_EXP($2,==)}
    | Exp PLUS error {MISSING_EXP($2,+)}
    | Exp MINUS error {MISSING_EXP($2,-)}
    | Exp MUL error {MISSING_EXP($2,*)}
    | Exp DIV error {MISSING_EXP($2,/)}
    | LP Exp RP {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | MINUS Exp %prec UMINUS {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}
    | NOT Exp {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,2,$1,$2);}
    | ID LP Args RP {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,4,$1,$2,$3,$4);}
    | ID LP RP {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Exp LB Exp RB {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,4,$1,$2,$3,$4);}
    | Exp LB Exp error {MISSING_RB($3)}
    | Exp DOT ID {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | ID {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | INT {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | FLOAT {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | CHAR {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    | LP Exp error {MISSING_RP($2)}
    | ID LP error {MISSING_RP($2)}
    | Exp UNKNOWN Exp {}
    | UNKNOWN {}
    ;
Args: Exp COMMA Args {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,3,$1,$2,$3);}
    | Exp {$$=new_node("Exp","",$1->lineno,NONTERMINAL); link_nodes($$,1,$1);}
    ;
%%
void yyerror(const char *s) {
    // fprintf(stderr, "%s ---%d\n", s, yylineno);
    error_flag=1;
}
int main(int argc, char **argv){
    char *file_path;
    // -o output file
    // -i intermdeia file

    char intermdedia[128]={},output[128]={},input[128]={},c;

    while ((c=getopt(argc,argv,"i:o:"))!=-1){
        switch (c){
            case 'i':
                strcpy(intermdedia,optarg);
                break;
            case 'o':
                strcpy(output,optarg);
                break;
        }
    }
    strcpy(input,argv[optind]);
    
    if (output[0]==0){
        char* dot=strrchr(input,'.');
        strcpy(dot,".out");
        strcpy(output,input);
    }

    int remain_arg = argc-optind;

    if(remain_arg < 1){
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return EXIT_FAIL;
    } else if(remain_arg == 1){


        file_path = argv[optind];
        if(!(yyin = fopen(file_path, "r"))){
            perror(argv[optind]);
            return EXIT_FAIL;
        }

        char *processed=preprocess(file_path);

        if (processed==NULL){
            fprintf(stderr,"error on preprocess\n");
	    	return EXIT_FAIL;
        }

        if (intermdedia[0]!=0){
            FILE *f=fopen(intermdedia,"w");
            fputs(processed,f);
            fclose(f);
        }

        YY_BUFFER_STATE bp = yy_scan_string(processed);
    
        //分配失败
        if (bp == NULL) {
	    	fprintf(stderr,"error on creating YY_BUFFER_STATE\n");
	    	return EXIT_FAIL;
	    }
	    //将输入源转为指定内存
	    yy_switch_to_buffer(bp);

        output_file=fopen(output,"w");

        int val=yyparse();
        if (error_flag==0) {
            print_tree(root,0,output_file);
        }

        fclose(output_file);
        
        yy_delete_buffer(bp);
	    yylex_destroy();
        return EXIT_OK;
    } else{
        fputs("Too many arguments! Expected: 1.\n", stderr);
        return EXIT_FAIL;
    }
}