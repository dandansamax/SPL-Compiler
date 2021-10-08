#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "preprocess.h"

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

int add_include(IncludeNode *head, char *filename, int line_number)
{
  IncludeNode *cur_node = head;
  while (cur_node->next != head)
  {
    cur_node = cur_node->next;
    if (!strcmp(cur_node->file_name, filename))
    {
      return 0;
    }
  }
  IncludeNode *new_node = (IncludeNode *)malloc(sizeof(IncludeNode));
  new_node->file_name = filename;
  new_node->line_number = line_number;
  new_node->pre = head->pre;
  new_node->next = head;
  head->pre->next = new_node;
  head->pre = new_node;
  return 1;
}

int parse_define(MacroNode *head, char *def_line, int line_number)
{
  char name[1000];
  char arg_name[100];
  ArgNode *arg_head = (ArgNode *)malloc(sizeof(ArgNode));
  arg_head->pre = arg_head->next = arg_head;
  arg_head->arg_name = NULL;
  int index = 0;
  int inside = 0;
  int ready_for_arg = 0;
  int arg_count = 0;
  char c;
  while ((c = input()) == ' ')
  {
  };
  do
  {
    if (c == '\n')
    {
      printf("Macro error at line %d", line_number);
      return 0;
    }
    else if (c == '(')
    {
      if (inside)
      {
        printf("Macro error at line %d", line_number);
        return 0;
      }
      else
      {
      
        inside = 1;
        ready_for_arg = 1;
      }
    }
    else if (c == ')')
    {
      if (!inside)
      {
        printf("Macro error at line %d", line_number);
        return 0;
      }
      else
      {
        inside = 0;
      }
    }
    else if (c == ',' && inside)
    {
      if (ready_for_arg)
      {
        printf("Macro error at line %d", line_number);
        return 0;
      }
      else
      {
        ready_for_arg = 1;
      }
    }
    else if (c == '')
      buffer[index++] = c;
  } while ((c = input()) != ' ' || inside);

  while ((c = input()) == ' ')
    ;
  do
  {
    buffer[index++] = c;
  } while ((c = input()) != '\n');
  return 1;
}

int parse_undefine(MacroNode *head, char *def_line, int line_number)
{
  return 1;
}

int parse_include(IncludeNode *head, char *def_line, int line_number)
{
  return 1;
}

void append_token(Token *head, char *value, int line_number)
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