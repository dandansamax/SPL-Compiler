#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "preprocess.h"

void link_include(IncludedNode *included_list, const char *included_file_path, FILE *fd)
{
  const char *included_filename = get_filename(included_file_path);
  if (is_included(included_list, included_filename))
  {
    printf("Duplicate include file: %s, ignored\n", included_filename);
    return;
  }
  add_included(included_list, included_filename);
  char buf[2048];
  FILE *fp = fopen(included_file_path, "r");
  if (fp == NULL)
  {
    perror("open file");
    exit(0);
  }
  while (fgets(buf, sizeof(buf), fp) != NULL)
  {
    if (!strncmp(buf, "#include", 8))
    {
      strtok(buf, "\"");
      char *inc_file = strtok(NULL, "\"");
      char inc_file_path[2048];
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

int def_new_macro(MacroNode *head, int is_object, char *value, char *sub, int start_line)
{
  MacroNode *cur_node = head;
  while (cur_node->next != head)
  {
    cur_node = cur_node->next;
    if (!strcmp(cur_node->value, value) && cur_node->end_line == -1)
    {
      // duplicate define
      return 0;
    }
  }
  MacroNode *new_node = (MacroNode *)malloc(sizeof(MacroNode));
  new_node->value = value;
  new_node->sub = sub;
  new_node->start_line = start_line;
  new_node->end_line = -1;
  new_node->pre = head->pre;
  new_node->next = head;
  head->pre->next = new_node;
  head->pre = new_node;
  return 1;
}

int undef_macro(MacroNode *head, char *value, int end_line)
{
  MacroNode *cur_node = head;
  while (cur_node->next != head)
  {
    cur_node = cur_node->next;
    if (!strcmp(cur_node->value, value) && cur_node->end_line == -1)
    {
      cur_node->end_line = end_line;
      return 1;
    }
  }
  // not defined
  return 0;
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

void add_included(IncludedNode *head, const char *filename)
{
  IncludedNode *new_node = (IncludedNode *)malloc(sizeof(IncludedNode));
  new_node->filename = filename;
  new_node->pre = head->pre;
  new_node->next = head;
  head->pre->next = new_node;
  head->pre = new_node;
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
  rel_pwd = (char *)malloc(sizeof(char) * (after_last_idx + 1));
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

void append_token(Token *head, char *value, int line_number, TokenType type)
{
  Token *new_node = (Token *)malloc(sizeof(Token));
  HideNode *new_hide_node = (HideNode *)malloc(sizeof(HideNode));
  new_hide_node->next = new_hide_node->pre = new_hide_node;
  new_hide_node->value = NULL;
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
}

void print_include(IncludedNode *head)
{
  IncludedNode *cur_node = head;
  while (cur_node->next != head)
  {
    cur_node = cur_node->next;
    printf("%s\n", cur_node->filename);
  }
}

// int main()
// {
//   IncludedNode *head = (IncludedNode *)malloc(sizeof(IncludedNode));
//   head->filename = NULL;
//   head->pre = head->next = head;
//   link_include(head, "test_1_r01.spl", "test1.lnk");
// }