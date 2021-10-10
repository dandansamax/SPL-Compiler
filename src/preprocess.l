%{
    #include "preprocess.h"
    Token *token_sequence;
%}

%option yylineno
%option noyywrap

define #define
undefine #undef
letter [a-zA-Z]
letter_ {letter}|_
digit [0-9]
id {letter_}({letter_}|{digit})*

%%

  //comment
"//" { char c; while((c=input()) != '\n'); }
"/*" { char c=input(); int state=0; char test;
    while(1){
        if (c==0){
            fprintf(stderr,"[FATAL] Comment error at line %d: cannot find \"*/\"\n",yylineno);
            exit(1);
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
            fprintf(stderr,"[FATAL] Comment error at line %d: multi-line comments cannot be nested\n",yylineno);
            exit(1);
            break;
        }
        c=input();
    }
}

\".*\"|\'.*\' { 
    char *buf = (char *)malloc(sizeof(char) * (yyleng + 1));
    strcpy(buf, yytext);
    append_token(token_sequence, buf, yylineno, STR); 
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
    append_token(token_sequence, buf, yylineno, PREPROCESS_ID);
}

[ ]+ {
    char *buf = (char *)malloc(sizeof(char) * (yyleng + 1));
    strcpy(buf, yytext);
    append_token(token_sequence, buf, yylineno, SPC);
}

. { 
    char *buf = (char *)malloc(sizeof(char) * (yyleng + 1));
    strcpy(buf, yytext);
    append_token(token_sequence, buf, yylineno, PREPROCESS_CHAR);
}

\n { 
    char *buf = (char *)malloc(sizeof(char) * (yyleng + 1));
    strcpy(buf, yytext);
    append_token(token_sequence, buf, yylineno, NL); 
}

%%
// int main(int argc, char **argv){
//     char *file_path;
//     if(argc < 2){
//         fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
//         return 1;
//     } else if(argc == 2)
//     {
//         char imm[5] = ".imm";
//         char imd_file[1024];
//         sprintf(imd_file, "%s%s", argv[1], imm);

//         // create the intermediate file
//         FILE *fp = fopen(imd_file, "w");
//         IncludedNode *included_set = (IncludedNode *)malloc(sizeof(IncludedNode));
//         included_set->filename = NULL;
//         included_set->pre = included_set->next = included_set;

//         // link the source code with the included files
//         link_include(included_set, argv[1], fp);
//         fclose(fp);
//         clear_included(included_set);
//         included_set = NULL;
//         if(!(yyin = fopen(imd_file, "r"))){
//             perror(imd_file);
//             return 0;
//         }
//         token_sequence = (Token *)malloc(sizeof(Token));
//         token_sequence->next = token_sequence->pre = token_sequence;
//         token_sequence->line_number = -1;
//         token_sequence->hide_set = NULL;
//         token_sequence->type = SUB;
//         token_sequence->value = NULL;
//         MacroNode *macro_set = (MacroNode *)malloc(sizeof(MacroNode));
//         macro_set->next = macro_set->pre = macro_set;
//         macro_set->macro = NULL;
//         macro_set->sub = NULL;

//         // break the source code into pieces
//         yylex();

//         // expand the macros
//         expand(token_sequence, macro_set);
//         clear_macro_set(macro_set);
//         macro_set = NULL;

//         // print the source codes
//         print_token(token_sequence);

//         // delete the intermediate file
//         remove(imd_file);
//         return 0;
//     } else
//     {
//         fputs("Too many arguments! Expected: 2.\n", stderr);
//         return 1;
//     }
// }
char* preprocess(char *file_path){
        char imm[5] = ".imm";
        char imd_file[1024];
        sprintf(imd_file, "%s%s", file_path, imm);
        
        // create the intermediate file
        FILE *fp = fopen(imd_file, "w");
        IncludedNode *included_set = (IncludedNode *)malloc(sizeof(IncludedNode));
        included_set->filename = NULL;
        included_set->pre = included_set->next = included_set;

        // link the source code with the included files
        link_include(included_set, file_path, fp);
        fclose(fp);
        clear_included(included_set);
        included_set = NULL;
        if(!(yyin = fopen(imd_file, "r"))){
            perror(imd_file);
            return NULL;
        }
        token_sequence = (Token *)malloc(sizeof(Token));
        token_sequence->next = token_sequence->pre = token_sequence;
        token_sequence->line_number = -1;
        token_sequence->hide_set = NULL;
        token_sequence->type = SUB;
        token_sequence->value = NULL;

        // break the source code into pieces
        yylex();

        // expand the macros
        expand(token_sequence);

        // delete the intermediate file
        remove(imd_file);

        // print the source codes
        return print_token(token_sequence);

        // return 0;
}