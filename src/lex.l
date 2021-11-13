%{
    #define EXIT_OK 0
    #define EXIT_FAIL 1


    #include "utils/tokentree.h"

    #define YYSTYPE Node *

    int error_flag=0;
    FILE *output_file;

    #include "syntax.tab.h"

    static char tmpstr[12];

%}

%option yylineno
%option noyywrap

letter [a-zA-Z]
letter_ {letter}|_
digit [0-9]
hex_digit [0-9a-fA-F]
number [1-9]{digit}*|0
hex_number 0[xX]([1-9a-fA-F]{hex_digit}*|0)


%%

"function" {yylval=new_node("FUNC_VAR","", yylineno, OTHER_TOKEN,0); return FUNC_VAR;}
"struct" {yylval=new_node("STRUCT","", yylineno, OTHER_TOKEN,0); return STRUCT;}
"if" {yylval=new_node("IF","", yylineno, OTHER_TOKEN,0); return IF;}
"else" {yylval=new_node("ELSE","", yylineno, OTHER_TOKEN,0); return ELSE;}
"while" {yylval=new_node("WHILE","", yylineno, OTHER_TOKEN,0); return WHILE;}
"return" {yylval=new_node("RETURN","", yylineno, OTHER_TOKEN,0); return RETURN;}
"for" {yylval=new_node("FOR","", yylineno, OTHER_TOKEN,0); return FOR;}
\. {yylval=new_node("DOT","", yylineno, OTHER_TOKEN,0); return DOT;}
; {yylval=new_node("SEMI","", yylineno, OTHER_TOKEN,0); return SEMI;}
, {yylval=new_node("COMMA","", yylineno, OTHER_TOKEN,0); return COMMA;}
= {yylval=new_node("ASSIGN","", yylineno, OTHER_TOKEN,0); return ASSIGN;}
\< {yylval=new_node("LT","", yylineno, OTHER_TOKEN,0); return LT;}
\<= {yylval=new_node("LE","", yylineno, OTHER_TOKEN,0); return LE;}
\> {yylval=new_node("GT","", yylineno, OTHER_TOKEN,0); return GT;}
\>= {yylval=new_node("GE","", yylineno, OTHER_TOKEN,0); return GE;}
!= {yylval=new_node("NE","", yylineno, OTHER_TOKEN,0); return NE;}
== {yylval=new_node("EQ","", yylineno, OTHER_TOKEN,0); return EQ;}
\+ {yylval=new_node("PLUS","", yylineno, OTHER_TOKEN,0); return PLUS;}
- {yylval=new_node("MINUS","", yylineno, OTHER_TOKEN,0); return MINUS;}
\* {yylval=new_node("MUL","", yylineno, OTHER_TOKEN,0); return MUL;}
\/ {yylval=new_node("DIV","", yylineno, OTHER_TOKEN,0); return DIV;}
&& {yylval=new_node("AND","", yylineno, OTHER_TOKEN,0); return AND;}
\|\| {yylval=new_node("OR","", yylineno, OTHER_TOKEN,0); return OR;}
! {yylval=new_node("NOT","", yylineno, OTHER_TOKEN,0); return NOT;}
\( {yylval=new_node("LP","", yylineno, OTHER_TOKEN,0); return LP;}
\) {yylval=new_node("RP","", yylineno, OTHER_TOKEN,0); return RP;}
\[ {yylval=new_node("LB","", yylineno, OTHER_TOKEN,0); return LB;}
\] {yylval=new_node("RB","", yylineno, OTHER_TOKEN,0); return RB;}
\{ {yylval=new_node("LC","", yylineno, OTHER_TOKEN,0); return LC;}
\} {yylval=new_node("RC","", yylineno, OTHER_TOKEN,0); return RC;}

"int" {yylval=new_node("TYPE","int", yylineno, TYPE_TOKEN, 0); return TYPE;}
"float" {yylval=new_node("TYPE","float", yylineno, TYPE_TOKEN, 1); return TYPE;}
"char" {yylval=new_node("TYPE","char", yylineno, TYPE_TOKEN, 2); return TYPE;}

{letter_}({letter_}|{digit})* {yylval = new_node("ID", strdup(yytext), yylineno, ID_TOKEN, 0); return ID;} //ID

\'[[:print:]]?\' {yylval = new_node("CHAR", strdup(yytext), yylineno, INT_FLOAT_CHAR_TOKEN, 0); return CHAR;} //CHAR
\'\\[xX]{hex_digit}{hex_digit}\' {yylval = new_node("CHAR", strdup(yytext), yylineno, INT_FLOAT_CHAR_TOKEN,1); return CHAR;} //CHAR

{number}\.{digit}+ {yylval = new_node("FLOAT", strdup(yytext), yylineno, INT_FLOAT_CHAR_TOKEN,0); return FLOAT;} //FLOAT

{hex_number} {
    int val;
    sscanf(yytext,"%x",&val);
    sprintf(tmpstr,"%d",val);
    yylval = new_node("INT", strdup(tmpstr), yylineno, INT_FLOAT_CHAR_TOKEN,0);
    return INT;
} //INT heximal

{number} { yylval = new_node("INT", strdup(yytext), yylineno, INT_FLOAT_CHAR_TOKEN,1); return INT; } //INT

[\n\t\r ]+ { /* does nothing when seeing white spaces except new line */ }

    /* Error handler */
{digit}({letter_}|{digit})* {fprintf(output_file,"Error type A at Line %d: unknown lexeme %s\n",yylineno,yytext); error_flag=1; return ID;} //Error id
\'\\[xX]({digit}|{letter_})*\' {fprintf(output_file,"Error type A at Line %d: unknown lexeme %s\n",yylineno,yytext); error_flag=1; return CHAR;} //Error char
. {fprintf(output_file,"Error type A at Line %d: unknown lexeme %s\n",yylineno,yytext); error_flag=1; return UNKNOWN;}

%%
