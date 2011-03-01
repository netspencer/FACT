#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

//////////////////
// Enumerations. 
//////////////////

typedef enum _TYPE_DEFINE
  {   
    VAR_TYPE      ,
    FUNCTION_TYPE ,
    ERROR_TYPE    ,
  } type_define;

typedef enum _word_codes
  {
    PARSING_ERROR = -1 ,
    END                , // 0
    UNKNOWN            , // 1
    NEWLINE            ,
    COMB_ARR           ,
    PLUS               ,                    
    MINUS              ,
    MULTIPLY           ,
    DIVIDE             ,
    MOD                ,
    NEG                ,
    ADD_ASSIGN         ,
    SUB_ASSIGN         ,
    MULT_ASSIGN        ,
    DIV_ASSIGN         ,
    MOD_ASSIGN         ,
    AT                 ,
    SET                ,
    DEF                ,
    DEFUNC             ,
    FUNC_RET           ,
    FUNC_OBJ           ,
    COMMA              ,
    IN_SCOPE           ,
    OP_CURLY           ,
    CL_CURLY           ,
    OP_BRACKET         ,
    NOP_BRACKET        ,
    CL_BRACKET         ,
    OP_PAREN           ,
    CL_PAREN           ,
    QUOTE              ,
    AND                ,
    OR                 ,
    BIT_AND            ,
    BIT_IOR            ,
    BIT_XOR            ,
    EQ                 ,
    NEQ                ,
    LESS               ,
    MORE               ,
    LESS_EQ            ,
    MORE_EQ            ,
    VARIADIC           ,
    SIZE               ,
    IF                 ,
    ON_ERROR           ,
    WHILE              ,
    FOR                ,
    THEN               ,
    ELSE               ,
    SEMI               ,
    RETURN_STAT        ,
    BREAK_SIG          ,
    SPROUT             ,
  } word_code;

/////////////////////
// Data structures.
/////////////////////

/* mpc_t is used for arbitrary-precision arithmetic */
typedef struct
{
  mpz_t        object;
  unsigned int precision;
  
}mpc_t;

/* _VAR and var_t are the variable holding structures */
typedef struct  _VAR
{
  bool  locked;
  mpz_t array_size;
  mpc_t data;

  char *name;

  struct _VAR *next;
  struct _VAR *array_up;
  
} var_t;

// _FUNC and func_t are the function holding structures
typedef struct _FUNC
{  
  int   line;
  bool  locked;
  mpz_t array_size;
  
  char   *name;
  char   *file_name;
  char  **args;
  char  **body;
  void   *usr_data;
  var_t  *vars;

  void *(*extrn_func)(struct _FUNC *);

  struct _FUNC *funcs; 
  struct _FUNC *up;
  struct _FUNC *caller;
  struct _FUNC *array_up;
  struct _FUNC *next;

  struct _MIXED
  {
    type_define type;
    
    var_t *var_p;
    
    struct _FUNC  *func_p;
    struct _MIXED *next;
    
  } *variadic;
  
} func_t;

// _ERROR holds thrown errors to be caught
typedef struct 
{
  bool thrown;
  
  char   *description;
  func_t *scope;
  
} _ERROR;

// Ambigious structure that holds a var, func, and error, along with other data.
typedef struct 
{

  bool        break_signal;
  bool        return_signal;
  _ERROR      error;
  type_define type;
  
  var_t  *v_point;
  func_t *f_point;
  
} FACT_t;
  
// Used for evaluating. Someday I hope this will be completely removed.
typedef struct
{
  int   *lines;
  char **syntax;
  
} word_list;

// Used for parsing.
typedef struct _LINKED_WORD
{
  int       newlines;
  word_code code;

  char *physical; // physical is only used if code is equal to UNKNOWN, otherwise it is NULL.
  
  struct _LINKED_WORD *hidden;
  struct _LINKED_WORD *hidden_up;
  struct _LINKED_WORD *next;
  struct _LINKED_WORD *previous;
  
} linked_word;

struct elements
{
  char    *name;
  char    *arguments;
  FACT_t (*function)(func_t *);
};

// BIF - structure that defines a BIF's arguments and function.
typedef struct
{
  char *arguments;
  void *(*function)(func_t *);
} BIF;

#endif

