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

{include}\s+ { parse_include(include_set, yytext, yylineno); }
{define}\s+ {
    char buff[100];
    int macro_len = 0;
    int state = 0;
    end = 0;
    char c;
    while (!end) {
        switch (state) {
            case 0:
                c = input();
                if (isalpha(c) || c == '_')
                {
                    buff[macro_len++] = c;
                    state = 1;
                }
                else {
                    printf("Error Macro at line %d", yylineno);
                    return;
                }
                break;
            case 1:
                c = input();
                if (isalnum(c) || c == '_')
                {
                    buff[macro_len++] = c;
                    state = 1;
                } else if (c == '\n')
                {
                    printf("Error Macro at line %d", yylineno);
                    return;
                } else {
                    end = 1;
                }
        }
    }
    switch (state) {
        case 0:

    if(isalpha(c) || c == '_'){

    }
            break;
    }
    if(isalpha(c) || c == '_'){

    }
    parse_define(macro_set, yytext, yylineno); }
{undefine}\s+ { parse_undefine(macro_set, yytext, yylineno); }
{id} { 
    char *buf = (char *)malloc(sizeof(char) * (yyleng + 1));
    strcpy(buf, yytext);
    append_token(token_sequence, buf, yylineno); 
}
. { 
    char *buf = (char *)malloc(sizeof(char) * (yyleng + 1));
    strcpy(buf, yytext);
    append_token(token_sequence, buf, yylineno); 
}
\n { 
    char *buf = (char *)malloc(sizeof(char) * (yyleng + 1));
    strcpy(buf, yytext);
    append_token(token_sequence, buf, yylineno); 
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
