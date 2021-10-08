%{
    #include "preprocess.h"

    struct MacroNode *macro_set;
    struct IncludeNode *include_set;
    struct Token *token_sequence;

%}

%option yylineno

define #define
undefine #undef
include #include
letter [a-zA-Z]
letter_ {letter}|_
digit [0-9]
id {letter_}({letter_}|{digit})*

%%

{include} {
    char *buf = (char *)malloc(sizeof(char) * (yyleng + 1));
    strcpy(buf, yytext);
    append_token(token_sequence, buf, yylineno, INC);
}
{define} {
    char *buf = (char *)malloc(sizeof(char) * (yyleng + 1));
    strcpy(buf, yytext);
    append_token(token_sequence, buf, yylineno, DEF);
}
{undefine} {
    char *buf = (char *)malloc(sizeof(char) * (yyleng + 1));
    strcpy(buf, yytext);
    append_token(token_sequence, buf, yylineno, UNDEF);
}
{id} { 
    char *buf = (char *)malloc(sizeof(char) * (yyleng + 1));
    strcpy(buf, yytext);
    append_token(token_sequence, buf, yylineno, ID);
}
. { 
    char *buf = (char *)malloc(sizeof(char) * (yyleng + 1));
    strcpy(buf, yytext);
    append_token(token_sequence, buf, yylineno, CHAR);
}
\n { 
    char *buf = (char *)malloc(sizeof(char) * (yyleng + 1));
    strcpy(buf, yytext);
    append_token(token_sequence, buf, yylineno, NL); 
}

%%
int main(int argc, char **argv){
    token_sequence = (Token *)malloc(sizeof(Token));
    token_sequence->next = token_sequence->pre = token_sequence;
    token_sequence->line_number = -1;
    token_sequence->value = NULL;
    char *file_path;
    if(argc < 2){
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return 0;
    } else if(argc == 2){
        file_path = argv[1];
        if(!(yyin = fopen(file_path, "r"))){
            perror(argv[1]);
            return 0;
        }
        yylex();
        print_token(token_sequence);
        return 1;
    } else{
        fputs("Too many arguments! Expected: 2.\n", stderr);
        return 0;
    }
}
