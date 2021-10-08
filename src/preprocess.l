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
hex_digit [0-9a-fA-F]
non_hex_digit[^0-9a-fA-F]
number [1-9]{digit}*|0
hex_number 0[xX]([1-9a-fA-F]{hex_digit}*|0)

%%

{include}.*\n { parse_include(include_set, yytext, yylineno); }
{define}.*\n { parse_define(macro_set, yytext, yylineno); }
{undefine}.*\n { parse_undefine(macro_set, yytext, yylineno); }
\n|struct|if|else|while|return|\.|;|,|=|\<|\<=|\>|\>=|!=|==|\+|-|\*|\/|&&|\|\||!|\(|\)|\[|\]|\{|\}|int|float|char|{letter_}({letter_}|{digit})*|\'[[:print:]]?\'|\'\\[xX]|{number}\.{digit}+|{hex_number}|{number}|[\n\t\r ]+|{digit}({letter_}|{digit})*|\'\\[xX]({digit}|{letter_})*\'|. { append_token(token_sequence, yytext, yylineno); }

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
