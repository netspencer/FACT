#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

/* define all enum types to make my life better ---- */

typedef enum _TYPE_DEFINE
  {   
    VAR_TYPE = 0  ,
    FUNCTION_TYPE ,
    ERROR_TYPE    , 
  } type_define;

typedef enum _word_codes
  {
    PARSING_ERROR = -1 ,
    END                , /* 0 */
    UNKNOWN            , /* 1 */
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
  } word_code;

/* end enums ---- */

/* define all data structures ---- */

/* mpc_t is used for arbitrary-precision arithmetic */
typedef struct
{
  unsigned int precision ;
  mpz_t        object    ;
}              mpc_t     ;

/* _VAR and var_t are the variable holding structures */
typedef struct  _VAR
{
  bool          locked     ;
  char        * name       ;
  mpz_t         array_size ;
  mpc_t         data       ;
  struct _VAR * array_up   ;
  struct _VAR * next       ;
}               var_t      ;

/* _FUNC and func_t are the function holding structures */ 
typedef struct     _FUNC
{  
  int              line      ;
  bool             locked    ;
  char         *   name      ;
  char         *   file_name ;
  char         **  args      ;
  char         **  body      ;
  void         *   usr_data  ;
  void         *(* extrn_func)(struct _FUNC *);
  var_t        *   vars       ;
  mpz_t            array_size ;
  struct _FUNC *   funcs      ; 
  struct _FUNC *   up         ;
  struct _FUNC *   caller     ;
  struct _FUNC *   array_up   ;
  struct _FUNC *   next       ;
  struct _MIXED
  {
    type_define     type   ;
    var_t         * var_p  ;
    struct _FUNC  * func_p ;
    struct _MIXED * next   ;
  } * variadic             ;
}                  func_t  ;

/* _ERROR holds thrown errors to be caught */
typedef struct 
{
  bool     thrown      ;
  char   * description ;
  func_t * scope       ;
}          _ERROR      ;

/* Ambigious structure that holds a var, func, and error and other data */
typedef struct 
{
  bool          return_signal ;
  bool          break_signal  ;
  type_define   type          ;
  var_t       * v_point       ;
  func_t      * f_point       ;
  _ERROR        error         ;
}               FACT_t        ;

/* Used for evaluating. Someday I hope this will be removed. */
typedef struct
{
  int  *  lines        ;
  char ** syntax       ;
}         word_list    ;

typedef struct          _LINKED_WORD
{
  int                   newlines    ;
  /* physical is only used if code is equal to UNKNOWN, otherwise it is NULL. */
  char                * physical    ;
  word_code             code        ;
  struct _LINKED_WORD * hidden      ;
  struct _LINKED_WORD * hidden_up   ;
  struct _LINKED_WORD * next        ;
  struct _LINKED_WORD * previous    ;
}                       linked_word ;

struct      elements
{
  char    * name;
  char    * arguments;
  FACT_t (* function)(func_t *);
};

/* end data structures ---- */

#endif

