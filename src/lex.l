%{
    #define EXIT_OK 0
    #define EXIT_FAIL 1


    #include "tokentree.h"

    #define YYSTYPE struct node *

    int error_flag=0;

    #include "syntax.tab.h"

    static char tmpstr[12];

%}

%option yylineno

letter [a-zA-Z]
letter_ {letter}|_
digit [0-9]
hex_digit [0-9a-fA-F]
non_hex_digit[^0-9a-fA-F]
number [1-9]{digit}*|0
hex_number 0[xX]([1-9a-fA-F]{hex_digit}*|0)


%%
  //comment
"//" { char c; while((c=input()) != '\n'); }
"/*" { char c=input(); int state=0; char test;
    while(1){
        if (c==0){
            printf("Error type A at Line %d: cannot find \"*/\"\n",yylineno);
            break;
        }
        switch (state){
        case 0:
            if (c=='*') state=1;
            else if (c=='/') state=2;
            break;
        case 1:
            if (c=='/') state=3;
            else if (c=='*') state=1;
            else state=0;
            break;
        case 2:
            if (c=='*') state=4;
            else if (c=='/') state=2;
            else state=0;
            break;
        }
        if (state==3){
            break;
        }
        else if (state==4){
            printf("Error type A at Line %d: multi-line comments cannot be nested\n",yylineno);
            error_flag=1;
            break;
        }
        c=input();
    }
}

"struct" {yylval=new_node("STRUCT","", yylineno, OTHER_TOKEN); return STRUCT;}
"if" {yylval=new_node("IF","", yylineno, OTHER_TOKEN); return IF;}
"else" {yylval=new_node("ELSE","", yylineno, OTHER_TOKEN); return ELSE;}
"while" {yylval=new_node("WHILE","", yylineno, OTHER_TOKEN); return WHILE;}
"return" {yylval=new_node("RETURN","", yylineno, OTHER_TOKEN); return RETURN;}
\. {yylval=new_node("DOT","", yylineno, OTHER_TOKEN); return DOT;}
; {yylval=new_node("SEMI","", yylineno, OTHER_TOKEN); return SEMI;}
, {yylval=new_node("COMMA","", yylineno, OTHER_TOKEN); return COMMA;}
= {yylval=new_node("ASSIGN","", yylineno, OTHER_TOKEN); return ASSIGN;}
\< {yylval=new_node("LT","", yylineno, OTHER_TOKEN); return LT;}
\<= {yylval=new_node("LE","", yylineno, OTHER_TOKEN); return LE;}
\> {yylval=new_node("GT","", yylineno, OTHER_TOKEN); return GT;}
\>= {yylval=new_node("GE","", yylineno, OTHER_TOKEN); return GE;}
!= {yylval=new_node("NE","", yylineno, OTHER_TOKEN); return NE;}
== {yylval=new_node("EQ","", yylineno, OTHER_TOKEN); return EQ;}
\+ {yylval=new_node("PLUS","", yylineno, OTHER_TOKEN); return PLUS;}
- {yylval=new_node("MINUS","", yylineno, OTHER_TOKEN); return MINUS;}
\* {yylval=new_node("MUL","", yylineno, OTHER_TOKEN); return MUL;}
\/ {yylval=new_node("DIV","", yylineno, OTHER_TOKEN); return DIV;}
&& {yylval=new_node("AND","", yylineno, OTHER_TOKEN); return AND;}
\|\| {yylval=new_node("OR","", yylineno, OTHER_TOKEN); return OR;}
! {yylval=new_node("NOT","", yylineno, OTHER_TOKEN); return NOT;}
\( {yylval=new_node("LP","", yylineno, OTHER_TOKEN); return LP;}
\) {yylval=new_node("RP","", yylineno, OTHER_TOKEN); return RP;}
\[ {yylval=new_node("LB","", yylineno, OTHER_TOKEN); return LB;}
\] {yylval=new_node("RB","", yylineno, OTHER_TOKEN); return RB;}
\{ {yylval=new_node("LC","", yylineno, OTHER_TOKEN); return LC;}
\} {yylval=new_node("RC","", yylineno, OTHER_TOKEN); return RC;}

"int" {yylval=new_node("TYPE",strdup("int"), yylineno, TYPE_TOKEN); return TYPE;}
"float" {yylval=new_node("TYPE",strdup("float"), yylineno, TYPE_TOKEN); return TYPE;}
"char" {yylval=new_node("TYPE",strdup("char"), yylineno, TYPE_TOKEN); return TYPE;}

{letter_}({letter_}|{digit})* {yylval = new_node("ID", strdup(yytext), yylineno, ID_TOKEN); return ID;} //ID

\'[[:print:]]?\' {yylval = new_node("CHAR", strdup(yytext), yylineno, INT_FLOAT_CHAR_TOKEN); return CHAR;} //CHAR
\'\\[xX]{hex_digit}{hex_digit}\' {yylval = new_node("CHAR", strdup(yytext), yylineno, INT_FLOAT_CHAR_TOKEN); return CHAR;} //CHAR

{number}\.{digit}+ {yylval = new_node("FLOAT", strdup(yytext), yylineno, INT_FLOAT_CHAR_TOKEN); return FLOAT;} //FLOAT

{hex_number} {
    int val;
    sscanf(yytext,"%x",&val);
    sprintf(tmpstr,"%d",val);
    yylval = new_node("INT", strdup(tmpstr), yylineno, INT_FLOAT_CHAR_TOKEN);
    return INT;
} //INT heximal

{number} { yylval = new_node("INT", strdup(yytext), yylineno, INT_FLOAT_CHAR_TOKEN); return INT; } //INT

[\n\t\r ]+ { /* does nothing when seeing white spaces except new line */ }

    /* Error handler */
{digit}({letter_}|{digit})* {printf("Error type A at Line %d: unknown lexeme %s\n",yylineno,yytext); error_flag=1; return ID;} //Error id
\'\\[xX]({digit}|{letter_})*\' {printf("Error type A at Line %d: unknown lexeme %s\n",yylineno,yytext); error_flag=1; return CHAR;} //Error char
. {printf("Error type A at Line %d: unknown lexeme %s\n",yylineno,yytext); error_flag=1; return UNKNOWN;}

%%
/* int main(int argc, char **argv){
    char *file_path;
    if(argc < 2){
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return EXIT_FAIL;
    } else if(argc == 2){
        file_path = argv[1];
        if(!(yyin = fopen(file_path, "r"))){
            perror(argv[1]);
            return EXIT_FAIL;
        }
        yylex();
        return EXIT_OK;
    } else{
        fputs("Too many arguments! Expected: 2.\n", stderr);
        return EXIT_FAIL;
    }
}*/
