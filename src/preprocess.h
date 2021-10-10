#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define MAX_SIZE 4*1024*1024

typedef enum TokenType
{
  DEF = 1,
  UNDEF = 2,
  PREPROCESS_ID = 3,
  PREPROCESS_CHAR = 4,
  NL = 5,
  STR = 6,
  SPC = 7,
  SUB = 8
} TokenType;

typedef struct HideNode
{
  struct HideNode *next, *pre;
  const char *macro;
} HideNode;

typedef struct Token
{
  struct Token *next, *pre;
  HideNode *hide_set;
  TokenType type;
  const char *value;
  int line_number;
} Token;

typedef struct MacroNode
{
  struct MacroNode *next, *pre;
  const char *macro;
  Token *sub;
} MacroNode;

typedef struct IncludedNode
{
  struct IncludedNode *next, *pre;
  const char *filename;
} IncludedNode;

void link_include(IncludedNode *included_list, const char *included_file, FILE *fd);

void expand(Token *token_sequence);

Token *remove_token(Token *token);

Token *get_sub(MacroNode *head, const char *macro);

Token *generate_sub_sequence(const Token *head, const HideNode *hide_set);

Token *parse_define(const Token *head, Token *cur_token, MacroNode *macro_set);

Token *parse_undefine(const Token *head, Token *cur_token, MacroNode *macro_set);

Token *substitute(Token *cur_token, MacroNode *macro_set);

void def_new_macro(MacroNode *head, const char *macro, Token *sub, int line_number);

void undef_macro(MacroNode *head, const char *macro, int line_number);

int is_defined(MacroNode *head, const char *macro);

void clear_macro_set(MacroNode *head);

void add_hide_node(HideNode *head, const char *macro);

int is_hidden(HideNode *head, const char *macro);

HideNode *copy_hide_set(const HideNode *head);

void add_included(IncludedNode *head, const char *filename);

int is_included(IncludedNode *head, const char *filename);

void clear_included(IncludedNode *head);

char *get_rel_path(const char *file_path);

char *get_filename(const char *file_path);

void append_token(Token *head, const char *value, int line_number, TokenType type);

char *print_token(Token *head);

char* preprocess(char*);