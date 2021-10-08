typedef enum TokenType
{
  ID,
  DEF,
  UNDEF,
  INC,
  CHAR,
  NL
} TokenType;

typedef struct HideNode
{
  struct HideNode *next, *pre;
  char *value;
} HideNode;

typedef struct Token
{
  struct Token *next, *pre;
  HideNode *hide_set;
  TokenType type;
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
  int arg_count;
  ArgNode *arg_list;
  char *value;
  char *sub;
  int start_line;
  int end_line;
} MacroNode;

typedef struct IncludedNode
{
  struct IncludedNode *next, *pre;
  const char *filename;
} IncludedNode;

void link_include(IncludedNode *included_list, const char *included_file, FILE *fd);

void expand();

int in_set();

int def_new_macro(MacroNode *head, int is_object, char *value, char *sub, int start_line);

int undef_macro(MacroNode *head, char *value, int end_line);

int is_included(IncludedNode *head, const char *filename);

void add_included(IncludedNode *head, const char *filename);

char *get_rel_path(const char *file_path);

char *get_filename(const char *file_path);

int parse_define(MacroNode *head, char *def_line, int line_number);

int parse_undefine(MacroNode *head, char *def_line, int line_number);

void append_token(Token *head, char *value, int line_number, TokenType type);

void print_token(Token *head);

void print_include(IncludedNode *head);