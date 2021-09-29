%{
    #define EXIT_OK 0
    #define EXIT_FAIL 1

    #define YYSTYPE char*

    #include"syntax.tab.h"

%}

%option yylineno

letter [a-zA-Z]
letter_ {letter}|_
digit [0-9]
hex_digit [0-9a-f]
number [1-9]{digit}*|0
hex_number 0[xX]([1-9a-f]{hex_digit}*|0)

%%

struct {return STRUCT;}
if {return IF;}
else {return ELSE;}
while {return WHILE;}
return {return RETURN;}
\. {return DOT;}
; {return SEMI;}
, {return COMMA;}
= {return ASSIGN;}
\< {return LT;}
\<= {return LE;}
\> {return GT;}
\>= {return GE;}
!= {return NE;}
== {return EQ;}
\+ {return PLUS;}
- {return MINUS;}
\* {return MUL;}
\/ {return DIV;}
&& {return AND;}
\|\| {return OR;}
! {return NOT;}
\( {return LP;}
\) {return RP;}
\[ {return LB;}
\] {return RB;}
\{ {return LC;}
\} {return RC;}

int|float|char { if (strcmp(yytext,"int")==0) yylval = strdup("int");
                     else if (strcmp(yytext,"float")==0) yylval = strdup("float");
                     else if (strcmp(yytext,"char")==0) yylval = strdup("char");
                 return TYPE;}

{letter_}({letter_}|{digit})* {yylval = strdup(yytext); return ID;} //ID

\'[[:print:]]?\' {yylval =  strdup(yytext); return CHAR;} //CHAR

{number}\.{digit}+ {yylval = strdup(yytext); return FLOAT;} //FLOAT

({number}|{hex_number}) {yylval = strdup(yytext); return INT;} //INT

[\n\t\r ]+ { /* does nothing when seeing white spaces except new line */ }
. {printf("Line %d: ERROR character %s\n",yylineno,yytext);}

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
