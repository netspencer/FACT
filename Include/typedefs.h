#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

//////////////////
// Enumerations. 
//////////////////

typedef enum FACT_type
  {   
    VAR_TYPE      ,
    FUNCTION_TYPE ,
    ERROR_TYPE    ,
  } type_t;

/////////////////////
// Data structures.
/////////////////////

// mpc_t is used for arbitrary-precision arithmetic
typedef struct
{
  mpz_t        object;
  unsigned int precision;  
} mpc_t;

// FACT_var and var_t are the variable holding structures
typedef struct FACT_var
{
  bool  locked;
  mpz_t array_size;
  mpc_t data;

  char *name;
  struct FACT_var *next;
  struct FACT_var *array_up;
} var_t;

typedef struct FACT_func
{  
  int   line;
  bool  locked;
  mpz_t array_size;

  int   *lines;
  char  *name;
  char  *file_name;
  char  **args;
  char  **body;
  void  *usr_data;
  var_t *vars;

  void *(*extrn_func)(struct FACT_func *);
  struct FACT_func *funcs; 
  struct FACT_func *up;
  struct FACT_func *caller;
  struct FACT_func *array_up;
  struct FACT_func *next;
  struct FACT_mixed
  {
    type_t type;
    
    var_t *var_p;
    struct FACT_func  *func_p;
    struct FACT_mixed *next;
  } *variadic;
} func_t;

typedef struct FACT_error 
{
  int line;
  char   *description;
  func_t *scope;  
} error_t;

// Ambigious structure that holds a var, func, and error, along with other data.
typedef struct 
{
  bool    break_signal;
  bool    return_signal;
  type_t  type;
  error_t error;
  
  var_t  *v_point;
  func_t *f_point;  
} FACT_t;
  
// Used for evaluating. Someday I hope this will be completely removed.
typedef struct FACT_syn_tree
{
  int  *lines;
  char *base;
  char **syntax;  
} syn_tree_t;

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

