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
"//" { char c; while((c=input()) != '\n'); append_token(token_sequence, "\n", yylineno, NL);}
"/*" { char c=input(); int state=0; char test;
    while(1){
        if(c == '\n'){
            append_token(token_sequence, "\n", yylineno, NL);
        }
        if (c == 0){
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
    append_token(token_sequence, strdup(yytext), yylineno, STR); 
}

{define} {
    append_token(token_sequence, strdup(yytext), yylineno, DEF);
}

{undefine} {
    append_token(token_sequence, strdup(yytext), yylineno, UNDEF);
}

{id} {
    append_token(token_sequence, strdup(yytext), yylineno, PREPROCESS_ID);
}

[ ]+ {
    append_token(token_sequence, strdup(yytext), yylineno, SPC);
}

. {
    append_token(token_sequence, strdup(yytext), yylineno, PREPROCESS_CHAR);
}

\n {
    append_token(token_sequence, strdup(yytext), yylineno, NL); 
}

%%
char* preprocess(char *file_path)
{
        char lnk[5] = ".lnk";
        char lnk_file[1024];
        sprintf(lnk_file, "%s%s", file_path, lnk);
        
        // create the intermediate file
        FILE *fp = fopen(lnk_file, "w");
        IncludedNode *included_set = (IncludedNode *)malloc(sizeof(IncludedNode));
        included_set->filename = NULL;
        included_set->pre = included_set->next = included_set;

        // link the source code with the included files
        link_include(included_set, file_path, fp);
        fclose(fp);
        clear_included(included_set);
        included_set = NULL;
        if(!(yyin = fopen(lnk_file, "r")))
        {
            perror(lnk_file);
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
        remove(lnk_file);

        // print the source codes
        return print_token(token_sequence);

        // return 0;
}