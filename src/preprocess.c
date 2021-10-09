#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "preprocess.h"

void link_include(IncludedNode *included_list, const char *included_file_path, FILE *fd)
{
  int line_cnt = 0;
  const char *included_filename = get_filename(included_file_path);
  if (is_included(included_list, included_filename))
  {
    fprintf(stderr, "[Ignored] Duplicate included file: %s\n", included_filename);
    return;
  }
  add_included(included_list, included_filename);
  char buf[1024];
  FILE *fp = fopen(included_file_path, "r");
  if (fp == NULL)
  {
    perror("open file");
    exit(0);
  }
  while (fgets(buf, sizeof(buf), fp) != NULL)
  {
    line_cnt++;
    if (!strncmp(buf, "#include", 8))
    {
      int idx = 0, cnt = 0;
      char c = buf[idx++];
      while (c != '\0')
      {
        if (c == '\"')
        {
          cnt++;
        }
        c = buf[idx++];
      }
      if (cnt != 2)
      {
        fprintf(stderr, "[Ignored] Include error at file \"%s\" line %d: %s\n", included_file_path, line_cnt, buf);
        continue;
      }
      strtok(buf, "\"");
      char *inc_file = strtok(NULL, "\"");
      char *inc_file_path = (char *)malloc(sizeof(char) * 200);
      char *rel_path = get_rel_path(included_file_path);
      sprintf(inc_file_path, "%s%s", rel_path, inc_file);
      if (rel_path != NULL)
      {
        free(rel_path);
      }
      link_include(included_list, inc_file_path, fd);
      continue;
    }
    fputs(buf, fd);
  }
  fputs("\n", fd);
  fclose(fp);
}

void expand(Token *token_sequence, MacroNode *macro_set)
{
  Token *cur_node = token_sequence->next;
  while (cur_node != token_sequence)
  {
    if (cur_node->type == DEF)
    {
      cur_node = parse_define(token_sequence, cur_node, macro_set);
    }
    else if (cur_node->type == UNDEF)
    {
      cur_node = parse_undefine(token_sequence, cur_node, macro_set);
    }
    else if (cur_node->type == ID && is_hidden(cur_node->hide_set, cur_node->value))
    {
      cur_node = cur_node->next;
    }
    else if (cur_node->type == ID && is_defined(macro_set, cur_node->value))
    {
      cur_node = substitute(cur_node, macro_set);
    }
    else
    {
      cur_node = cur_node->next;
    }
  }
}

Token *remove_token(Token *token)
{
  Token *next_node = token->next;
  HideNode *cur_hide_node = token->hide_set->next;
  HideNode *next_hide_node = cur_hide_node;
  while (cur_hide_node != token->hide_set)
  {
    next_hide_node = cur_hide_node->next;
    free(cur_hide_node);
    cur_hide_node = next_hide_node;
  }
  token->pre->next = token->next;
  token->next->pre = token->pre;
  free(token->hide_set);
  free(token);
  return next_node;
}

Token *get_sub(MacroNode *head, const char *macro)
{
  MacroNode *cur_node = head;
  while (cur_node->next != head)
  {
    cur_node = cur_node->next;
    if (!strcmp(cur_node->macro, macro))
    {
      return cur_node->sub;
    }
  }
  return NULL;
}

Token *generate_sub_sequence(const Token *head, const HideNode *hide_set)
{
  Token *new_head = (Token *)malloc(sizeof(Token));
  new_head->pre = new_head->next = new_head;
  new_head->value = NULL;
  new_head->hide_set = NULL;
  new_head->type = SUB;
  Token *cur_node = head->next;
  while (cur_node != head)
  {
    append_token(new_head, cur_node->value, cur_node->line_number, cur_node->type);
    cur_node = cur_node->next;
  }
  cur_node = new_head->next;

  // set hide set for each token in the new sequence
  while (cur_node != new_head)
  {
    cur_node->hide_set = copy_hide_set(hide_set);
    cur_node = cur_node->next;
  }
  return new_head;
}

Token *parse_define(const Token *head, Token *cur_token, MacroNode *macro_set)
{
  int line_number = cur_token->line_number;
  cur_token = remove_token(cur_token);
  if (cur_token->type != SPC)
  {
    fprintf(stderr, "[Ignored] Macro error at line %d: invalid macro definition\n", line_number);
    // remove the remaining characters in this line
    while (cur_token != head && cur_token->type != NL)
    {
      cur_token = remove_token(cur_token);
    }
    if (cur_token != head)
    {
      cur_token = remove_token(cur_token);
    }
    return cur_token;
  }
  cur_token = remove_token(cur_token);
  if (cur_token->type != ID)
  {
    fprintf(stderr, "[Ignored] Macro error at line %d: invalid macro definition\n", line_number);
    // remove the remaining characters in this line
    while (cur_token != head && cur_token->type != NL)
    {
      cur_token = remove_token(cur_token);
    }
    if (cur_token != head)
    {
      cur_token = remove_token(cur_token);
    }
    return cur_token;
  }
  const char *macro = cur_token->value;
  cur_token = remove_token(cur_token);
  if (cur_token->type != SPC)
  {
    fprintf(stderr, "[Ignored] Macro error at line %d: invalid macro definition\n", line_number);
    // remove the remaining characters in this line
    while (cur_token != head && cur_token->type != NL)
    {
      cur_token = remove_token(cur_token);
    }
    if (cur_token != head)
    {
      cur_token = remove_token(cur_token);
    }
    return cur_token;
  }
  cur_token = remove_token(cur_token);
  Token *sub_head = (Token *)malloc(sizeof(Token));
  sub_head->pre = sub_head->next = sub_head;
  sub_head->hide_set = NULL;
  sub_head->line_number = -1;
  sub_head->type = SUB;
  sub_head->value = NULL;
  while (cur_token != head && cur_token->type != NL)
  {
    append_token(sub_head, cur_token->value, -1, cur_token->type);
    cur_token = remove_token(cur_token);
  }
  def_new_macro(macro_set, macro, sub_head, line_number);
  if (cur_token != head)
  {
    cur_token = remove_token(cur_token);
  }
  return cur_token;
}

Token *parse_undefine(const Token *head, Token *cur_token, MacroNode *macro_set)
{
  int line_number = cur_token->line_number;
  cur_token = remove_token(cur_token);
  if (cur_token->type != SPC)
  {
    fprintf(stderr, "[Ignored] Macro error at line %d: invalid macro undef\n", line_number);
    // remove the remaining characters in this line
    while (cur_token != head && cur_token->type != NL)
    {
      cur_token = remove_token(cur_token);
    }
    if (cur_token != head)
    {
      cur_token = remove_token(cur_token);
    }
    return cur_token;
  }
  cur_token = remove_token(cur_token);
  if (cur_token->type != ID)
  {
    fprintf(stderr, "[Ignored] Macro error at line %d: invalid macro undef\n", line_number);
    // remove the remaining characters in this line
    while (cur_token != head && cur_token->type != NL)
    {
      cur_token = remove_token(cur_token);
    }
    if (cur_token != head)
    {
      cur_token = remove_token(cur_token);
    }
    return cur_token;
  }
  const char *macro = cur_token->value;
  cur_token = remove_token(cur_token);
  if (cur_token->type != SPC && cur_token->type != NL && cur_token != head)
  {
    fprintf(stderr, "[Ignored] Macro error at line %d: invalid macro undef\n", line_number);
    // remove the remaining characters in this line
    while (cur_token != head && cur_token->type != NL)
    {
      cur_token = remove_token(cur_token);
    }
    if (cur_token != head)
    {
      cur_token = remove_token(cur_token);
    }
    return cur_token;
  }
  undef_macro(macro_set, macro, line_number);
  while (cur_token != head && cur_token->type != NL)
  {
    cur_token = remove_token(cur_token);
  }
  if (cur_token != head)
  {
    cur_token = remove_token(cur_token);
  }
  return cur_token;
}

Token *substitute(Token *cur_token, MacroNode *macro_set)
{
  add_hide_node(cur_token->hide_set, cur_token->value);
  Token *sub = generate_sub_sequence(get_sub(macro_set, cur_token->value), cur_token->hide_set);
  sub->pre->next = cur_token->next;
  sub->next->pre = cur_token;
  cur_token->next->pre = sub->pre;
  cur_token->next = sub->next;
  free(sub);
  cur_token = remove_token(cur_token);
  return cur_token;
}

void def_new_macro(MacroNode *head, const char *macro, Token *sub, int line_number)
{
  if (is_defined(head, macro))
  {
    fprintf(stderr, "[Ignored] Macro error at line %d: %s can not be defined twice\n", line_number, macro);
    return;
  }
  MacroNode *new_node = (MacroNode *)malloc(sizeof(MacroNode));
  new_node->sub = sub;
  new_node->macro = macro;
  new_node->pre = head->pre;
  new_node->next = head;
  head->pre->next = new_node;
  head->pre = new_node;
}

void undef_macro(MacroNode *head, const char *macro, int line_number)
{
  MacroNode *cur_node = head;
  while (cur_node->next != head)
  {
    cur_node = cur_node->next;
    if (!strcmp(cur_node->macro, macro))
    {
      cur_node->pre->next = cur_node->next;
      cur_node->next->pre = cur_node->pre;
      free(cur_node);
      return;
    }
  }
  fprintf(stderr, "[Ignored] Macro error at line %d: %s can not be undefined because it's not defined\n", line_number, macro);
}

int is_defined(MacroNode *head, const char *macro)
{
  MacroNode *cur_node = head;
  while (cur_node->next != head)
  {
    cur_node = cur_node->next;
    if (!strcmp(cur_node->macro, macro))
    {
      return 1;
    }
  }
  return 0;
}

void clear_macro_set(MacroNode *head)
{
  MacroNode *cur_node = head->next;
  free(head);
  MacroNode *next_node = cur_node;
  while (cur_node != head)
  {
    next_node = cur_node->next;
    free(cur_node);
    cur_node = next_node;
  }
}

void add_hide_node(HideNode *head, const char *macro)
{
  HideNode *new_node = (HideNode *)malloc(sizeof(HideNode));
  new_node->macro = macro;
  new_node->pre = head->pre;
  new_node->next = head;
  head->pre->next = new_node;
  head->pre = new_node;
}

int is_hidden(HideNode *head, const char *macro)
{
  HideNode *cur_node = head;
  while (cur_node->next != head)
  {
    cur_node = cur_node->next;
    if (!strcmp(cur_node->macro, macro))
    {
      return 1;
    }
  }
  return 0;
}

HideNode *copy_hide_set(const HideNode *head)
{
  HideNode *new_head = (HideNode *)malloc(sizeof(HideNode));
  new_head->pre = new_head->next = new_head;
  new_head->macro = NULL;
  HideNode *cur_node = head->next;
  while (cur_node != head)
  {
    add_hide_node(new_head, cur_node->macro);
    cur_node = cur_node->next;
  }
  return new_head;
}

void add_included(IncludedNode *head, const char *filename)
{
  IncludedNode *new_node = (IncludedNode *)malloc(sizeof(IncludedNode));
  new_node->filename = filename;
  new_node->pre = head->pre;
  new_node->next = head;
  head->pre->next = new_node;
  head->pre = new_node;
}

int is_included(IncludedNode *head, const char *filename)
{
  IncludedNode *cur_node = head;
  while (cur_node->next != head)
  {
    cur_node = cur_node->next;
    if (!strcmp(cur_node->filename, filename))
    {
      return 1;
    }
  }
  return 0;
}

void clear_included(IncludedNode *head)
{
  IncludedNode *cur_node = head->next;
  free(head);
  IncludedNode *next_node = cur_node;
  while (cur_node != head)
  {
    next_node = cur_node->next;
    free(cur_node);
    cur_node = next_node;
  }
}

char *get_rel_path(const char *file_path)
{
  int idx = 0;
  int after_last_idx = 0;
  char c = file_path[idx++];
  while (c != '\0')
  {
    if (c == '/')
    {
      after_last_idx = idx;
    }
    c = file_path[idx++];
  }
  char *rel_pwd;
  rel_pwd = (char *)malloc(sizeof(char) * idx);
  strcpy(rel_pwd, file_path);
  rel_pwd[after_last_idx] = '\0';
  return rel_pwd;
}

char *get_filename(const char *file_path)
{
  int idx = 0;
  int after_last_idx = 0;
  char c = file_path[idx++];
  while (c != '\0')
  {
    if (c == '/')
    {
      after_last_idx = idx;
    }
    c = file_path[idx++];
  }
  char *filename = (char *)malloc(sizeof(char) * idx);
  strcpy(filename, file_path + after_last_idx);
  return filename;
}

void append_token(Token *head, const char *value, int line_number, TokenType type)
{
  Token *new_node = (Token *)malloc(sizeof(Token));
  HideNode *new_hide_node = (HideNode *)malloc(sizeof(HideNode));
  new_hide_node->next = new_hide_node->pre = new_hide_node;
  new_hide_node->macro = NULL;
  new_node->type = type;
  new_node->value = value;
  new_node->line_number = line_number;
  new_node->hide_set = new_hide_node;
  new_node->pre = head->pre;
  new_node->next = head;
  head->pre->next = new_node;
  head->pre = new_node;
}

void print_token(Token *head)
{
  Token *cur_node = head;
  while (cur_node->next != head)
  {
    cur_node = cur_node->next;
    printf("%s", cur_node->value);
  }
  printf("\n");
}