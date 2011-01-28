/*
 * parser.c
 * (c) 2010 Matthew Plant
 * Includes functions for parsing input into
 * more easily used data.
 */

#include "parser.h"

char *
add_newlines (char *word, int newlines)
{
  int    index;
  int    word_len;
  char * new_str;

  /* add_newlines - this function, given a string and a value,
   * returns a string allocated in memory of the passed string
   * combined with the value in newlines at the begining.
   * For example: word = "hello, world!", newlines = "2" would
   * return "\n\nhello, world!"
   */

  if (word[0] == BYTECODE)
    {
      if (word[1] == NUMBER)
	word_len = (sizeof (var_t *) / sizeof (char)) + 2;
      else
	word_len = 3;
    }
  else
    word_len = strlen (word);
  new_str  = better_malloc (sizeof (char) * (word_len + newlines + 1));

  for (index = 0; index < newlines; index++)
    new_str[index] = '\n';

  while (index < word_len + newlines)
    new_str[index++] = word[index - newlines];
  new_str[index] = '\0';

  return new_str;
}

char *
lookup_word (int code, int newlines)
{
  /* lookup_word - given a word code and a number of newlines,
   * look up the code in our trusty table and combine it with
   * the number of newlines.
   * IMPORTANT NOTE: code is NOT a word_code type, and does not
   * have values for END, UNKNOWN, etc. If you wish to use this
   * function by passing a word_code variable, you must first
   * subtract it by COMB_ARRAY.
   */
  
  static char * lookup_table [] =
    {
      "~"       ,
      "+"       ,
      "-"       ,
      "*"       ,
      "/"       ,
      "%"       ,
      "--"      ,
      "+="      ,
      "-="      ,
      "*="      ,
      "/="      ,
      "%="      ,
      "@"       ,
      "="       ,
      "def"     ,
      "defunc"  ,
      "$"       ,
      "&"       ,
      ","       ,
      ":"       ,
      "{"       ,
      "}"       ,
      "["       ,
      "!["      ,
      "]"       ,
      "("       ,
      ")"       ,
      "\""      ,
      "&&"      ,
      "||"      ,
      "`"       , 
      "|"       ,
      "^"       , 
      "=="      ,
      "!="      , 
      "<"       ,
      ">"       ,
      "<="      ,
      ">="      ,
      "->"      ,
      "sizeof"  ,
      "if"      ,
      "on_error",
      "while"   ,
      "for"     ,
      "then"    ,
      "else"    , 
      ";"       ,
      "return"  ,
      "break"   ,
    };

  return add_newlines (lookup_table[code], newlines);
}

static bool
is_in_quotes (int character)
{
  static bool prev_slash;
  static bool is_quote;

  /* is_in_quotes - given a character, check whether or not it is
   * currently in double quotes (") and return true if it is and 
   * false otherwise.
   * This may look really confusing, but it operates relatively
   * simply. is_quote is true when an opening " is passed.
   * Thus, when a " is passed, if is_quote is false, we turn it on.
   * prev_slash is true when a backwards slash (\) is passed.
   * If a \ is passed and prev_slash is set to true, prev_slash
   * is set to false, denoting a \\. Therefor, when a " is passsed
   * and prev_slash is true, we do not set is_quote to false, as
   * it denotes \".
   */
  
  switch (character)
    {
    case '"':
      if (!prev_slash)
	is_quote = !is_quote;
      return !is_quote;
      
    case '\0':
      return false;
      
    case '\\':
      prev_slash = !prev_slash;
      break;

    default:
      prev_slash = false;
      break;
    }
  
  return is_quote;
}

static bool
isopt (int op1, int op2)
{
  /* isopt - returns true when passed characters of the
   * following sequences:
   *  ==
   *  +=
   *  -=
   *  *=
   *  /=
   *  %=
   *  <=
   *  >=
   *  !=
   *  &&
   *  ||
   *  ->
   * Or returns false otherwise.
   */
  if (((op1 == '='
	|| op1 == '+'
	|| op1 == '-'
	|| op1 == '*'
	|| op1 == '/'
	|| op1 == '%'
	|| op1 == '<'
	|| op1 == '>'
	|| op1 == '!') && op2 == '=')  
      || (op1 == '&' && op2 == '&')
      || (op1 == '|' && op2 == '|')
      || (op1 == '-' && op2 == '>'))
    return true;
  return false;
}

char **
get_words (char *start)
{
  int     index; 
  int     jndex; 
  bool    is_string;
  char *  end;
  char ** result;

  /* get_words - the main tokenizing routine. I wrote this code
   * when I was really obsessed with being simplistic, and now
   * it looks like this might be the most confusing function in
   * the entire project.
   * Basically, given a string, this function allocates memory
   * for and returns a char **, with each place in char * pointing
   * to a token. It parses on some simple grammar:
   *  1. Strings become seperated into three tokens - ", the text, and " again.
   *  2. Spaces/tabs/newlines seperate tokens, unless inside a string.
   *  3. Anthing that is returned true by the isopt function becomes its
   *     own token.
   *  4. If the ispunct function returns true, then the current character
   *     gets its own token, with the exception of the '.'.
   *  5. Strings of letters, numbers, and periods (.) are all tokens.
   *
   * Variables:
   *  @result - the allocated char ** pointing to the different tokens
   *            in memory. This is the function's return value. 
   *  @index  - the current token being parsed.
   *  @jndex  - the position in of the current character in the new string.
   *  @start  - pointer to the start of the current token in the passed 
   *            string (also is the passed string itself).
   *  @end    - pointer to the end of the current token in the passed
   *            string. This gets incremented if the parsing grammar
   *            allows it, thus increasing the length of the token.
   * @is_string - this variable is set to false after creating a string
   *              token. That way, the ending " does not create a new
   *              string token.
   */
  
  for (index = 0, end = start, is_string = false, result = NULL; *end != '\0'; index++)
    {
      result = better_realloc (result, (index + 1) * sizeof (char *));

      while (isspace ((int) *end) && !is_in_quotes ((int) *end) && *end != '\n' && *end != '\0')
	start = ++end;

      if (*end == '\n')
	{
	  while (*end == '\n')
	    end++;
	}
      else if (is_string)
	{
	  is_string = false;
	  end++;
	}
      else if (is_in_quotes ((int) *end))
	{
	  do
	    end++;
	  while (is_in_quotes ((int) *end));
	  end--;
	  is_string = true;
	}
      else if (isopt ((int) *end, (int) *(end + 1)))
	end += 2;
      else if (ispunct ((int) *end) && *end != '.')
	end++;
      else
	{
	  while (isalnum (*end) || *end == '_' || *end == '.')
	    end++;
	}
      
      if ((end - start) > 0)
	{
	  result[index] = (char *) better_malloc ((end - start + 1) * sizeof(char));
	  for (jndex = 0; start != end; start++, jndex++)
            result[index][jndex] = *start;
	  result[index][jndex] = '\0';
	  start                = end;
	}
    }
      
  result[index - 1] = NULL;
  return (index > 0) ? result : NULL;
}

word_code
get_block_code (char *block)
{
  /* get_block_code - takes in a string, and returns the word_code
   * type of that string. For example, the string "+" would return PLUS.
   */
  
  if (block == NULL || *block == '\0')
    return END;
  else if (*block == '\n')
    return NEWLINE;
  else if (!strcmp (block, "~"))
    return COMB_ARR;
  else if (!strcmp (block, "+"))
    return PLUS;
  else if (!strcmp (block, "-"))
    return MINUS;
  else if (!strcmp (block, "*"))
    return MULTIPLY;
  else if (!strcmp (block, "/"))
    return DIVIDE;
  else if (!strcmp (block, "%"))
    return MOD;
  else if (!strcmp (block, "+="))
    return ADD_ASSIGN;
  else if (!strcmp (block, "-="))
    return SUB_ASSIGN;
  else if (!strcmp (block, "*="))
    return MULT_ASSIGN;
  else if (!strcmp (block, "/="))
    return DIV_ASSIGN;
  else if (!strcmp (block, "%="))
    return MOD_ASSIGN;
  else if (!strcmp (block, "@"))
    return AT;
  else if (!strcmp (block, "="))
    return SET;
  else if (!strcmp (block, "def"))
    return DEF;
  else if (!strcmp (block, "defunc"))
    return DEFUNC;
  else if (!strcmp (block, "$"))
    return FUNC_RET;
  else if (!strcmp (block, "&"))
    return FUNC_OBJ;
  else if (!strcmp (block, ":"))
    return IN_SCOPE;
  else if (!strcmp (block, ","))
    return COMMA;
  else if (!strcmp (block, "{"))
    return OP_CURLY;
  else if (!strcmp (block, "}"))
    return CL_CURLY;
  else if (!strcmp (block, "["))
    return OP_BRACKET;
  else if (!strcmp (block, "!["))
    return NOP_BRACKET;
  else if (!strcmp (block, "]"))
    return CL_BRACKET;
  else if (!strcmp (block, "("))
    return OP_PAREN;
  else if (!strcmp (block, ")"))
    return CL_PAREN;
  else if (!strcmp (block, "\""))
    return QUOTE;
  else if (!strcmp (block, /*"and"*/ "&&"))
    return AND;
  else if (!strcmp (block, /*"or"*/ "||"))
    return OR;
  else if (!strcmp (block, "`"))
    return BIT_AND;
  else if (!strcmp (block, "|"))
    return BIT_IOR;
  else if (!strcmp (block, "^"))
    return BIT_XOR;
  else if (!strcmp (block, /*"eq"*/ "=="))
    return EQ;
  else if (!strcmp (block, /*"nq"*/ "!="))
    return NEQ;
  else if (!strcmp (block, "<"))
    return LESS;
  else if (!strcmp (block, ">"))
    return MORE;
  else if (!strcmp (block, "<="))
    return LESS_EQ;
  else if (!strcmp (block, ">="))
    return MORE_EQ;
  else if (!strcmp (block, "->"))
    return VARIADIC;
  else if (!strcmp (block, "sizeof"))
    return SIZE;
  else if (!strcmp (block, "if"))
    return IF;
  else if (!strcmp (block, "on_error"))
    return ON_ERROR;
  else if (!strcmp (block, "while"))
    return WHILE;
  else if (!strcmp (block, "for"))
    return FOR;
  else if (!strcmp (block, "then"))
    return THEN;
  else if (!strcmp (block, "else"))
    return ELSE;
  else if (!strcmp (block, ";"))
    return SEMI;
  else if (!strcmp (block, "return"))
    return RETURN_STAT;
  else if (!strcmp (block, "break"))
    return BREAK_SIG;
  else
    return UNKNOWN;
}
  
static inline linked_word *
alloc_word (linked_word *set_prev)
{
  /* alloc_word - allocated the necessary space for a single
   * linked_word structure and initialize the default values.
   * I'm pretty sure that better_malloc returns everything
   * pre-initialized to zero, but whatever.
   */
  
  linked_word * temp;

  temp              = better_malloc (sizeof (linked_word));
  temp->newlines    = 0;
  temp->code        = UNKNOWN;
  temp->physical    = NULL;
  temp->hidden      = NULL;
  temp->hidden_up   = NULL;
  temp->next        = NULL;
  temp->previous    = set_prev;

  return temp;
}

linked_word *
create_list (char **words)
{
  word_code     w_code;
  linked_word * base;

  for (base = alloc_word (NULL); (w_code = get_block_code (words[0])) != END; words++)
    {
      if (w_code == NEWLINE)
	{
	  base->newlines = strlen (words[0]);
	  continue;
	}
      if (w_code == UNKNOWN)
	base->physical = words[0];

      base->code     = w_code;
      base->next     = alloc_word (base);
      base           = base->next;
    }
  base->code = END;

  /* I don't think this is needed, may remove it. */
  while (base->previous != NULL)
    base = base->previous;

  return base;
}

linked_word *
set_list (linked_word *start, word_code stopper) 
{
  word_code     w_code;
  linked_word * temp_link;
  
  while ((w_code = start->code) != stopper && w_code != END)
    {
      if (stopper == QUOTE)
	{
	  start = start->next;
	  continue;
	}
      
      if (w_code == OP_CURLY)
	{
	  start->hidden         = start->next;
	  start->next->previous = NULL;
      	  temp_link             = set_list (start->next, CL_CURLY);

	  if (temp_link->code == END)
	    return temp_link;
	  
	  start->next               = temp_link->next;
	  temp_link->next->previous = start;
	  temp_link->next           = NULL;
	  start->hidden->hidden_up  = start;
	}
      else if (w_code == OP_BRACKET)
	{
	  start->hidden         = start->next;
	  start->next->previous = NULL;
	  temp_link             = set_list (start->next, CL_BRACKET);

	  if (temp_link->code == END)
	    return temp_link;
	  
	  start->next               = temp_link->next;
	  temp_link->next->previous = start;
	  temp_link->next           = NULL;
	  start->hidden->hidden_up  = start;
	}
      else if (w_code == OP_PAREN)
	{
	  start->hidden         = start->next;
	  start->next->previous = NULL;
	  temp_link             = set_list (start->next, CL_PAREN);

	  if (temp_link->code == END)
	    return temp_link;
	  
	  start->next               = temp_link->next;
	  temp_link->next->previous = start;
	  temp_link->next           = NULL;

	  start->hidden->hidden_up = start;
	}
      else if (w_code == DEF
	       || w_code == DEFUNC)
	{
	  start->hidden         = start->next;
	  start->next->previous = NULL;
	  temp_link             = set_list (start->next, UNKNOWN);

	  if (temp_link->code == END)
	    return start;
	  
	  start->next               = temp_link->next;
	  temp_link->next->previous = start;
	  temp_link->next           = NULL;
	  start->hidden->hidden_up  = start;
	}
      else if (w_code == QUOTE)
	{
	  start->hidden         = start->next;
	  start->next->previous = NULL;
	  temp_link             = set_list (start->next, QUOTE);

	  if (temp_link->code == END)
	    return temp_link;
	  
	  start->next               = temp_link->next;
	  temp_link->next->previous = start;
	  temp_link->next           = NULL;
	  start->hidden->hidden_up  = start;
	}

      start = start->next;
    }
  
  return start;
}

void
swap (linked_word *swapping)
{
  int           hold_prev_lines;
  bool          hold_end;
  linked_word * temp_next;
  linked_word * temp_prev;

  if (swapping->previous == NULL)
    return;

  if (swapping->previous->hidden_up != NULL)
    {
      swapping->hidden_up           = swapping->previous->hidden_up;
      swapping->hidden_up->hidden   = swapping;
      swapping->previous->hidden_up = NULL;
    }

  hold_prev_lines                 = swapping->previous->newlines;
  swapping->previous->newlines    = swapping->newlines;
  swapping->newlines              = hold_prev_lines;
  
  temp_next                    = swapping->next;
  temp_prev                    = swapping->previous->previous;
  swapping->previous->next     = temp_next;
  swapping->previous->previous = swapping;
  swapping->next               = swapping->previous;
  swapping->previous           = temp_prev;

  if (temp_next != NULL)
    temp_next->previous = swapping->next;

  if (temp_prev != NULL)
    temp_prev->next = swapping;
}

#define isnotMDM(op) (op != MULTIPLY && op != DIVIDE && op != MOD)
#define isnotAS(op)  (op != COMB_ARR && op != PLUS && op != MINUS)
#define isnotSE(op)  (op != SET)
#define isnotOPC(op) (op != OP_CURLY)
#define isnotASN(op) (op < ADD_ASSIGN || op > MOD_ASSIGN)
#define isnotCMP(op) (op < AND || op > MORE_EQ)
#define isFACTop(op) ((op >= PLUS && op <= MOD_ASSIGN) || op == SET || (op >= AND && op <= MORE_EQ) || op == IN_SCOPE)

static void
set_neg_array (linked_word *scan)
{
  while (scan != NULL && scan->next != NULL)
    {
      /*
      if (scan->code == MINUS
      && */
      if ((scan->previous == NULL
	   || (scan->previous->code >= COMB_ARR
	       && scan->previous->code <= MOD_ASSIGN)
	   || scan->previous->code == SET
	   || scan->previous->code == COMMA
	   || scan->previous->code == IN_SCOPE
	   || (scan->previous->code >= AND
	       && scan->previous->code <= MORE_EQ)
	   || scan->previous->code == THEN
	   || scan->previous->code == RETURN_STAT))
	{
	  if (scan->code == MINUS)
	    scan->code = NEG;
	  else if (scan->code == OP_BRACKET
		   && (scan->hidden_up == NULL
		       || (scan->hidden_up->code == DEF || scan->hidden_up->code == DEFUNC)))
	    scan->code = NOP_BRACKET;
	}
      scan = scan->next;
    }
}
	
	  
static void
precedence_level1 (linked_word *scan)
{
  int           pos;
  linked_word * find_end;
  linked_word * move_along;

  while (scan != NULL && scan->next != NULL)
    {
      switch (scan->code)
	{
	case OP_CURLY:
	case NOP_BRACKET:
	case OP_BRACKET:
	case OP_PAREN:
	  //case DEF:
	  //case DEFUNC:
	  rev_shunting_yard (scan->hidden);
	  break;

	case IF:
	case WHILE:
	  if (scan->next->code == OP_PAREN)
	    {
	      rev_shunting_yard (scan->next->hidden);
	      scan->next->next->previous = NULL;
	      rev_shunting_yard (scan->next->next);
	      
	      for (find_end = scan->next->next;
		   find_end->previous != NULL;
		   find_end = find_end->previous);
	      
	      scan->next->next   = find_end;
	      find_end->previous = scan->next;
	    }
	  else
	    {
	      scan->next->previous = NULL;
	      rev_shunting_yard (scan->next->next);
	      
	      for (find_end = scan->next;
		   find_end->previous != NULL;
		   find_end = find_end->previous);
	      
	      scan->next         = find_end;
	      find_end->previous = scan;
	    }
	  return;

	 
	case FUNC_RET:
	case FUNC_OBJ:
	  scan->hidden                 = scan->next;
	  scan->next                   = scan->next->next;
	  scan->hidden->hidden_up      = scan;
	  scan->hidden->previous       = NULL;
	  scan->hidden->next->previous = scan;
	  scan->hidden->next           = NULL;
	  rev_shunting_yard ((scan->hidden->hidden != NULL) ? scan->hidden->hidden : NULL);
	  break;

	case FOR:
	case THEN:
	case ELSE:
	case COMMA:
	case RETURN_STAT:
	case CL_CURLY:
	case SEMI:
	  scan->next->previous = NULL;
	  rev_shunting_yard (scan->next);

	  for (find_end = scan->next;
	       find_end->previous != NULL;
	       find_end = find_end->previous);

	  scan->next         = find_end;
	  find_end->previous = scan;
	  return;
	  
	default:
	  break;
	}
      scan = scan->next;
    }
}

void
precedence_level2 (linked_word *scan)
{
  linked_word * hold;

  while (scan != NULL && scan->next != NULL)
    {
      switch (scan->code)
	{
	case IF:
	case WHILE:
	case FOR:
	case THEN:
	case ELSE:
	case COMMA:
	case RETURN_STAT:
	case CL_CURLY:
	case SEMI:
	  return;

	case IN_SCOPE:	 
	  hold = scan->next;
	  while (scan->previous != NULL
		 && scan->previous->code != IN_SCOPE
		 && scan->previous->code != BIT_AND
		 && scan->previous->code != BIT_IOR
		 && scan->previous->code != BIT_XOR
		 && isnotMDM (scan->previous->code)
		 && isnotAS  (scan->previous->code)
		 && isnotSE  (scan->previous->code)
		 && isnotOPC (scan->previous->code)
		 && isnotASN (scan->previous->code)
		 && isnotCMP (scan->previous->code))
	    swap (scan);
	  
	  scan = hold;
	  break;

	default:
	  break;
	}

      scan = scan->next;
    }
}

void
precedence_level3 (linked_word *scan)
{
  while (scan != NULL && scan->next != NULL)
    {
      switch (scan->code)
	{
	case IF:
	case WHILE:
	case FOR:
	case THEN:
	case ELSE:
	case IN_SCOPE:
	case COMMA:
	case RETURN_STAT:
	case CL_CURLY:
	case SEMI:
	  return;
	  
	case MULTIPLY:
	case DIVIDE:
	case MOD:
	  swap (scan);
	  break;

	default:
	  break;
	}

      scan = scan->next;
    }
}

void
precedence_level4 (linked_word *scan)
{
  linked_word * hold;

  while (scan != NULL && scan->next != NULL)
    {
      switch (scan->code)
	{
	case IF:
	case WHILE:
	case FOR:
	case THEN:
	case ELSE:
	case IN_SCOPE:
	case COMMA:
	case RETURN_STAT:
	case CL_CURLY:
	case SEMI:
	  return;

	case COMB_ARR:
	case PLUS:
	  hold = scan->next;
	  while (scan->previous != NULL
		 && scan->previous->code != BIT_AND
		 && scan->previous->code != BIT_IOR
		 && scan->previous->code != BIT_XOR
		 && isnotAS (scan->previous->code)
		 && isnotSE (scan->previous->code)
		 && isnotOPC (scan->previous->code)
		 && isnotASN (scan->previous->code)
		 && isnotCMP (scan->previous->code))
	    swap (scan);

	  if (hold->code == MINUS)
	    hold = hold->next;
	  
	  scan = hold;
	  break;
	  
	case MINUS:
	  hold = scan->next;
	  while (scan->previous != NULL
		 && scan->previous->code != BIT_AND
		 && scan->previous->code != BIT_IOR
		 && scan->previous->code != BIT_XOR
		 && scan->previous->code != MINUS
		 && isnotSE (scan->previous->code)
		 && isnotOPC (scan->previous->code)
		 && isnotASN (scan->previous->code)
		 && isnotCMP (scan->previous->code))
	    swap (scan);

	  if (hold->code == MINUS)
	    hold = hold->next;
	  
	  scan = hold;
	  break;

	default:
	  break;
	}

      scan = scan->next;
    }
}

void
precedence_level5 (linked_word *scan)
{
  linked_word *hold;

  while (scan != NULL && scan->next != NULL)
    {
      switch (scan->code)
	{
	case IF:
	case WHILE:
	case FOR:
	case THEN:
	case ELSE:
	case IN_SCOPE:
	case COMMA:
	case RETURN_STAT:
	case CL_CURLY:
	case SEMI:
	  return;
	  
	case LESS:
	case MORE:
	case LESS_EQ:
	case MORE_EQ:
	  hold = scan->next;
	  while (scan->previous != NULL
		 && scan->previous->code != BIT_AND
		 && scan->previous->code != BIT_IOR
		 && scan->previous->code != BIT_XOR
		 && isnotSE (scan->previous->code)
		 && isnotOPC (scan->previous->code)
		 && isnotASN (scan->previous->code)
		 && isnotCMP (scan->previous->code))
	    swap (scan);

	  scan = hold;
	  break;

	default:
	  break;
	}

      scan = scan->next;
    }
}

void
precedence_level6 (linked_word *scan)
{
  linked_word *hold;

  while (scan != NULL && scan->next != NULL)
    {
      switch (scan->code)
	{
	case IF:
	case WHILE:
	case FOR:
	case THEN:
	case ELSE:
	case IN_SCOPE:
	case COMMA:
	case RETURN_STAT:
	case CL_CURLY:
	case SEMI:
	  return;
	  
	case EQ:
	case NEQ:
	  hold = scan->next;
	  while (scan->previous != NULL
		 && scan->previous->code != BIT_AND
		 && scan->previous->code != BIT_IOR
		 && scan->previous->code != BIT_XOR
		 && isnotSE (scan->previous->code)
		 && isnotOPC (scan->previous->code)
		 && isnotASN (scan->previous->code)
		 && (scan->previous->code < AND
		     || scan->previous->code > NEQ))
	    swap (scan);
	  
	  scan = hold;
	  break;

	default:
	  break;
	}

      scan = scan->next;
    }
}

/* IT'S SO ANNOYING HOW EACH BITWISE OPERATION HAS ITS OWN PRECEDENCE LEVEL.
 * JESUS.
 */

void
precedence_level7 (linked_word *scan)
{
  linked_word *hold;

  while (scan != NULL && scan->next != NULL)
    {
      switch (scan->code)
	{
	case IF:
	case WHILE:
	case FOR:
	case THEN:
	case ELSE:
	case IN_SCOPE:
	case COMMA:
	case RETURN_STAT:
	case CL_CURLY:
	case SEMI:
	  return;
	  
	case BIT_AND:
	  hold = scan->next;
	  while (scan->previous != NULL
		 && scan->previous->code != BIT_AND
		 && scan->previous->code != BIT_IOR
		 && scan->previous->code != BIT_XOR
		 && isnotSE (scan->previous->code)
		 && isnotOPC (scan->previous->code)
		 && isnotASN (scan->previous->code)
		 && (scan->previous->code < AND
		     || scan->previous->code > NEQ))
	    swap (scan);
	  
	  scan = hold;
	  break;

	default:
	  break;
	}

      scan = scan->next;
    }
}

void
precedence_level8 (linked_word *scan)
{
  linked_word *hold;

  while (scan != NULL && scan->next != NULL)
    {
      switch (scan->code)
	{
	case IF:
	case WHILE:
	case FOR:
	case THEN:
	case ELSE:
	case IN_SCOPE:
	case COMMA:
	case RETURN_STAT:
	case CL_CURLY:
	case SEMI:
	  return;
	  
	case BIT_XOR:
	  hold = scan->next;
	  while (scan->previous != NULL
		 && scan->previous->code != BIT_XOR
		 && scan->previous->code != BIT_IOR
		 && isnotSE (scan->previous->code)
		 && isnotOPC (scan->previous->code)
		 && isnotASN (scan->previous->code)
		 && (scan->previous->code < AND
		     || scan->previous->code > NEQ))
	    swap (scan);
	  
	  scan = hold;
	  break;

	default:
	  break;
	}

      scan = scan->next;
    }
}

void
precedence_level9 (linked_word *scan)
{
  linked_word *hold;

  while (scan != NULL && scan->next != NULL)
    {
      switch (scan->code)
	{
	case IF:
	case WHILE:
	case FOR:
	case THEN:
	case ELSE:
	case IN_SCOPE:
	case COMMA:
	case RETURN_STAT:
	case CL_CURLY:
	case SEMI:
	  return;
	  
	case BIT_IOR:
	  hold = scan->next;
	  while (scan->previous != NULL
		 && scan->previous->code != BIT_IOR
		 && isnotSE (scan->previous->code)
		 && isnotOPC (scan->previous->code)
		 && isnotASN (scan->previous->code)
		 && (scan->previous->code < AND
		     || scan->previous->code > NEQ))
	    swap (scan);
	  
	  scan = hold;
	  break;

	default:
	  break;
	}

      scan = scan->next;
    }
}

void
precedence_level10 (linked_word *scan)
{
  linked_word *hold;

  while (scan != NULL && scan->next != NULL)
    {
      switch (scan->code)
	{
	case IF:
	case WHILE:
	case FOR:
	case THEN:
	case ELSE:
	case IN_SCOPE:
	case COMMA:
	case RETURN_STAT:
	case CL_CURLY:
	case SEMI:
	  return;
	  
	case AND:
	  hold = scan->next;
	  while (scan->previous != NULL
		 && isnotSE (scan->previous->code) 
		 && isnotOPC (scan->previous->code)
		 && isnotASN (scan->previous->code)
		 && scan->previous->code != AND
		 && scan->previous->code != OR)
	    swap (scan);
	  
	  scan = hold;
	  break;

	default:
	  break;
	}

      scan = scan->next;
    }
}

void
precedence_level11 (linked_word *scan)
{
  linked_word *hold;

  while (scan != NULL && scan->next != NULL)
    {
      switch (scan->code)
	{
	case IF:
	case WHILE:
	case FOR:
	case THEN:
	case ELSE:
	case IN_SCOPE:
	case COMMA:
	case RETURN_STAT:
	case CL_CURLY:
	case SEMI:
	  return;
	  
	case OR:
	  hold = scan->next;
	  while (scan->previous != NULL
		 && isnotSE (scan->previous->code)
		 && isnotOPC (scan->previous->code)
		 && isnotASN (scan->previous->code)
		 && scan->previous->code != OR)
	    swap (scan);
	  
	  scan = hold;
	  break;

	default:
	  break;
	}

      scan = scan->next;
    }
}

void
precedence_level12 (linked_word *scan)
{
  linked_word *hold;

  while (scan != NULL && scan->next != NULL)
    {
      switch (scan->code)
	{
	case IF:
	case WHILE:
	case FOR:
	case THEN:
	case ELSE:
	case IN_SCOPE:
	case COMMA:
	case RETURN_STAT:
	case CL_CURLY:
	case SEMI:
	  return;
	  
	case SET:
	case ADD_ASSIGN:
	case SUB_ASSIGN:
	case MULT_ASSIGN:
	case DIV_ASSIGN:
	case MOD_ASSIGN:
	  hold = scan->next;
	  while (scan->previous != NULL
		 && isnotSE (scan->previous->code)
		 && isnotOPC (scan->previous->code)
		 && isnotASN (scan->previous->code))
	    swap (scan);
	  
	  scan = hold;
	  break;

	default:
	  break;
	}

      scan = scan->next;
    }
}

void
rev_shunting_yard (linked_word *scan)
{
  set_neg_array (scan);
  precedence_level1  (scan);
  precedence_level2  (scan);
  precedence_level3  (scan);
  precedence_level4  (scan);
  precedence_level5  (scan);
  precedence_level6  (scan);
  precedence_level7  (scan);
  precedence_level8  (scan);
  precedence_level9  (scan);
  precedence_level10 (scan);
  precedence_level11 (scan);
  precedence_level12 (scan);
}
  
void
set_end (linked_word *start,
	 linked_word *end)
{
  while (start->next != NULL)
    start = start->next;
  
  start->next = end;
  
  if (end != NULL)
    end->previous = start;
}

void
set_link (linked_word *scan)
{
  linked_word *temp_next;
  
  while (scan != NULL)
    {
      if (scan->hidden != NULL)
	{
	  temp_next              = scan->next;
	  scan->next             = scan->hidden;
	  scan->hidden->previous = scan;
	  set_end (scan->next, temp_next);
	}
       
      scan = scan->next;
    }
}  

char **
convert_link (linked_word *list)
{
  char         ** result;
  unsigned int    index;
        
  result = better_malloc (sizeof (char *));

  for (index = 0; list != NULL; list = list->next, index++)
    {
      if (list->code == UNKNOWN)
	result[index] = add_newlines (list->physical, list->newlines);
      else if (list->code > UNKNOWN)
	result[index] = lookup_word (list->code - COMB_ARR, list->newlines);

      result = better_realloc (result, sizeof (char **) * (index + 1));
    }

  result[index] = NULL;
  return result;
}

static inline int
get_end_block (int block)
{
  /* I'm not going to make this a giant return (cond) ? : okay? */
  if (block == '(')
    return ')';
  if (block == '['
      || block == '!')
    return ']';
  if (block == '{')
    return '}';
  /* NOTREACHED */
  return 0;
}

int
get_exp_length (char **words, int end_block)
{
  int lines;
  int index;

  for (index = 0; words[index] != NULL; index++)
    {
      for (lines = 0; words[index][lines] == '\n'; lines++);

      if (words[index][lines] == '\0'
	  || words[index][lines] == end_block)
	break;
      
      if (words[index][lines] == '('
	  || words[index][lines] == '['
	  || (words[index][lines] == '!'
	      && words[index][lines + 1] == '[')
	  || words[index][lines] == '{')
	index += get_exp_length (words + index + 1,
				 get_end_block (words[index][lines]));
    }

  return (words[index] == NULL) ? index : index + 1;
}

int
get_exp_length_first (char **words, int end_block)
{
  int lines;
  int index;

  for (index = 0; words[index] != NULL; index++)
    {
      for (lines = 0; words[index][lines] == '\n'; lines++);

      if (words[index][lines] == '\0'
	  || words[index][lines] == end_block)
	break;
      
      if (words[index][lines] == '('
	  || words[index][lines] == '['
	  || (words[index][lines] == '!'
	      && words[index][lines + 1] == '['))
	  
	index += get_exp_length (words + index + 1,
				 get_end_block (words[index][lines]));
      else if (words[index][lines] == '{')
	return index + get_exp_length (words + index + 1, '}') + 1;
    }

  return (words[index] == NULL) ? index : index + 1;
}
