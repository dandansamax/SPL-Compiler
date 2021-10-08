typedef struct HideNode
{
  struct HideNode *next, *pre;
  char *value;
} HideNode;

typedef struct Token
{
  struct Token *next, *pre;
  HideNode *hide_set;
  char *value;
  int line_number;
} Token;

typedef struct ArgNode
{
  struct ArgNode *next, *pre;
  char *arg_name;
} ArgNode;

typedef struct MacroNode
{
  struct MacroNode *next, *pre;
  int is_object;
  ArgNode *arg_list;
  char *value;
  char *sub;
  int start_line;
  int end_line;
} MacroNode;

typedef struct IncludeNode
{
  struct IncludeNode *next, *pre;
  char *file_name;
  int line_number;
} IncludeNode;



void expand();

int in_set();

int def_new_macro(MacroNode *head, int is_object, char *value, char *sub, int start_line);

int undef_macro(MacroNode *head, char *value, int end_line);

int add_include(IncludeNode *head, char *filename, int line_number);

int parse_define(MacroNode *head, char *def_line, int line_number);

int parse_undefine(MacroNode *head, char *def_line, int line_number);

int parse_include(IncludeNode *head, char *def_line, int line_number);

void append_token(Token *head, char *value, int line_number);

void print_token(Token *head);