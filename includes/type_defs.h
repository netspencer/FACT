#ifndef TYPE_DEFS_H_
#define TYPE_DEFS_H_

typedef struct
{
  unsigned int precision; /*
			    Unsigned as there will never be a
			    negative precision. By default the
			    value is 0, indicating 10^0 = 1.
			  */
  mpz_t object;           /*
			    All data is stored in the form of
			    an integer type.
			  */
} mpc_t;

typedef enum _TYPE_DEFINE
  {   
    VAR_TYPE = 0,
    FUNCTION_TYPE,
    ERROR_TYPE, 
  } type_define;

typedef struct _VAR
{ 
  char *name;
  int array_size;
  mpc_t data;
  struct _VAR *array_up;
  struct _VAR *next; 
} var_t;

typedef struct _FUNC
{
  char *name; 
  char **args;
  char **body;

  int array_size;

  var_t *vars;

  void * (*extrn_func)(struct _FUNC *);

  struct _FUNC *funcs; 
  struct _FUNC *up;
  struct _FUNC *array_up;
  struct _FUNC *next;
} func_t;

typedef struct 
{
  func_t *scope;
  char *description;
  bool thrown;
} _ERROR;

typedef struct 
{
  bool isret;
  bool break_signal;

  type_define type;

  var_t *v_point;

  func_t *f_point;

  _ERROR error;
} FACT_t;

typedef struct
{
  bool *move_forward;
  char **syntax;
} word_list;

typedef enum _word_codes
  {
    PARSING_ERROR = -1,
    END,                     /* -1 */
    UNKNOWN,                 /* 0  */
    PLUS,                    
    MINUS,
    MULTIPLY,
    DIVIDE,
    MOD,
    ADD_ASSIGN,
    SUB_ASSIGN,
    MULT_ASSIGN,
    DIV_ASSIGN,
    MOD_ASSIGN,
    AT,
    SET,
    DEF,
    DEFUNC,
    FUNC_RET,
    FUNC_OBJ,
    FUNC_END,
    COMMA,
    IN_SCOPE,
    OP_CURLY,
    CL_CURLY,
    OP_BRACKET,
    NOP_BRACKET,
    CL_BRACKET,
    OP_PAREN,
    CL_PAREN,
    QUOTE,
    AND,
    OR,
    EQ,
    NEQ,
    LESS,
    MORE,
    LESS_EQ,
    MORE_EQ,
    SIZE,
    IF,
    WHILE,
    FOR,
    THEN,
    ELSE,
    SEMI,
    RETURN_STAT,
  } word_code;

typedef struct _LINKED_WORD
{
  word_code code;

  char *physical; /* only used if code is equal to UNKNOWN, otherwise it is NULL. */

  struct _LINKED_WORD *hidden;
  struct _LINKED_WORD *hidden_up;
  struct _LINKED_WORD *next;
  struct _LINKED_WORD *previous;
} linked_word;

struct elements
{
  char *name;
  char *arguments;
  
  FACT_t (*function)(func_t *);
};

#endif