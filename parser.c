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
  int    i;
  int    word_len;
  char * new_str;

  word_len = strlen (word);
  new_str  = better_malloc (sizeof (char) * (word_len + newlines + 1));

  for (i = 0; i < newlines; i++)
    new_str[i] = '\n';

  while (i < word_len + newlines)
    new_str[i++] = word[i - newlines];
  new_str[i] = '\0';

  return new_str;
}

static char *
lookup_word (int code, int newlines)
{
  static char * lookup_table [] =
    {
      "~"       ,
      "+"       ,
      "-"       ,
      "*"       ,
      "/"       ,
      "%"       ,
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
      "sizeof"  ,
      "if"      ,
      "on_error",
      "while"   ,
      "for"     ,
      "then"    ,
      "else"    , 
      ";"       ,
      "return"  ,
    };

  return add_newlines (lookup_table[code], newlines);
}

static bool
is_in_quotes (int character)
{
  static bool prev_slash;
  static bool is_quote;
  
  switch (character)
    {
    case '"':
      if (!prev_slash)
	is_quote = !is_quote;
      return !is_quote;
      
    case '\0':
      return false;
      
    case '\\':
      prev_slash = true;
      break;

    default:
      prev_slash = false;
      break;
    }
  
  return is_quote;
}

static int
isopt (int character)
{
  /*
    need to fix this one up a bit.
    Probably would be a good idea to
    split up the operators into categories.
  */
  static int times = 0;
  
  switch (character)
    {
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '=':
    case '&':
    case '|':
    case '>':
    case '<':
    case '!':
    case '[':
      if (++times > 3)
	return (times = 0);
      return true;

    default:
      return (times = 0);
    }
}

char **
get_words (char *start)
{
  int     count;
  int     count2;
  bool    isstring;
  char *  end;
  char ** return_string;

  //return_string = (char **) better_malloc (sizeof (char *));
  return_string = NULL;
  
  for (count = 0, end = start, isstring = false; *end != '\0'; count++)
    {
      return_string = (char **) better_realloc (return_string, (count + 1) * sizeof (char *));

      while (isspace ((int) *end) && !is_in_quotes ((int) *end) && *end != '\n')
	start = ++end;

      if (*end == '\n')
	{
	  while (*end == '\n')
	    end++;
	}
      else if (isstring)
	{
	  isstring = false;
	  end++;
	}
      else if (is_in_quotes ((int) *end))
	{
	  while (is_in_quotes ((int) *end))
	    end++;
	  
	  end--;
	  isstring = true;
	}
      else if (isopt ((int) *end))
	{
	  while (isopt ((int) *end))
	    end++;
	}
      else if (ispunct ((int) *end) && *end != '.')
	end++;
      else
	{
	  while (isalnum (*end) || *end == '_' || *end == '.')
	    end++;
	}
      
      if ((end - start) > 0)
	{
	  return_string[count] = (char *) better_malloc ((end - start + 1) * sizeof(char));
	  
	  for (count2 = 0; start != end; start++, count2++)
            return_string[count][count2] = *start;
	  
	  return_string[count][count2] = '\0';
	  start                        = end;
	}
    }
      
  return_string[count - 1] = NULL;
  
  return (count > 0) ? return_string : NULL;
}

word_code
get_block_code (char *block)
{
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
  else if (!strcmp (block, /*"leq"*/ "<="))
    return LESS_EQ;
  else if (!strcmp (block, /*"meq"*/ ">="))
    return MORE_EQ;
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
  else
    return UNKNOWN;
}
  
static inline linked_word *
alloc_word (linked_word *set_prev)
{
  linked_word * temp;

  temp              = (linked_word *) better_malloc (sizeof (linked_word));
  temp->newlines    = 0;
  temp->code        = UNKNOWN;
  temp->is_negative = false;
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
      else if (w_code == OP_BRACKET
	       || w_code == NOP_BRACKET)
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
	    return temp_link;
	  
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

char *
parsing_error_set_get (char *new)
{
  /*
    If passed NULL, this function will
    return the last string. Else, it will
    set it to the one passed
  */
  static char * error = NULL;

  if (new == NULL)
    return error;
  else
    return (error = new);
}


bool
parsing_error (linked_word *scan, bool comma_ok,
	       unsigned char inside) // 0 = none, 1 = in paren, 2 = in bracket, 3 = in curly, 4 = in definition.
{
  typedef enum
  {
    NON_OPT ,
    OPT     , // also START
    FUNC    ,
    NEG     ,
    MKFUNC  ,
  } _prev_type;
  _prev_type prev_link;

  return false;

  
     //  This function does not work. It needs to be fixed.
    //  Not only does it not really work all that well, it
   // has yet to be golfed.
  

  /*
  for (prev_link = OPT; scan != NULL; scan = scan->next)
    {
      if (scan->code == UNKNOWN)
	parsing_error_set_get (combine_strs ("unexpected ", scan->physical));
      else if (scan->code != END)
	parsing_error_set_get (combine_strs ("unexpected ", lookup_word (scan->code - COMB_ARR)));	

      if (scan->code == AT)
	{
	  if (prev_link != OPT)
	    return true;
	  prev_link = MKFUNC;
	}
      else if (scan->code == FUNC_RET
	       || scan->code == FUNC_OBJ)
	{
	  if (prev_link != OPT && prev_link != NEG)
	    return true;
	  prev_link = FUNC;
	}
      else if (scan->code == DEF || scan->code == DEFUNC)
	{
	  if ((prev_link != OPT && prev_link != NEG && prev_link != MKFUNC)
	      || (parsing_error (scan->hidden, false, 4)))
	    return true;
	  prev_link = (prev_link == MKFUNC) ? MKFUNC : NON_OPT;
	}
      else if (scan->code == CL_CURLY)
	{
	  if (inside == 3 && prev_link == OPT)
	    return false;
	  else
	    return true;
	}
      else if (scan->code == CL_BRACKET)
	{
	  if (inside == 2 && prev_link == NON_OPT)
	    return false;
	  else
	    return true;
	}
      else if (scan->code == CL_PAREN)
	{
	  if (inside == 1 && prev_link == NON_OPT)
	    return false;
	  else
	    return true;
	}   
      else if (scan->code == UNKNOWN || scan->code == QUOTE)
	{
	  if (prev_link == NON_OPT || prev_link == FUNC)
	    return true;
	  if (inside == 4)
	    return false;
	  prev_link = NON_OPT;
	}
      else if (isFACTop (scan->code))
	{
	  if (prev_link == OPT)
	    {
	      if (scan->code == MINUS)
		prev_link = NEG;
	      else
		return true;
	    }
	  else if (prev_link == NEG || inside == 4)
	    return true;
	  else
	    prev_link = OPT;
	}
      else if (scan->code == OP_CURLY)
	{
	  if ((prev_link != NON_OPT && prev_link != MKFUNC)
	      || parsing_error (scan->hidden, false, 3))
	    return true;
	  prev_link = NON_OPT;
	}
      else if (scan->code == OP_BRACKET)
	{
	  if (parsing_error (scan->hidden, false, 2))
	    return true;
	  if (inside != 4)
	    prev_link = OPT;	    
	}
      else if (scan->code == NOP_BRACKET)
	{
	  if (prev_link != NEG
	      || parsing_error (scan->hidden, true, 2))
	    return true;
	  prev_link = NON_OPT;
	}
      else if (scan->code == OP_PAREN)
	{
	  if (prev_link == MKFUNC)
	    {
	      if (parsing_error (scan->hidden, true, 1))
		return true;
	    }
	  else if (prev_link == FUNC)
	    {
	      if (parsing_error (scan->hidden, true, 1))
		return true;
	      prev_link = NON_OPT;
	    }
	  else if (prev_link != OPT && prev_link != NEG)
	    return true; 
	  else if (parsing_error (scan->hidden, false, 1))
	    return true;
	  else
	    prev_link = NON_OPT;
	}
      else if (scan->code == COMMA)
	{
	  if (!comma_ok || prev_link == OPT || prev_link == NEG)
	    return true;
	  prev_link = OPT;
	}
      else if (scan->code == SEMI)
	{
	  if (prev_link == OPT || prev_link == NEG || inside == 1 || inside == 2)
	    return true;
	  else if (inside == 0)
	    return false;
	  else
	    prev_link = OPT;
	}
      else if (scan->code == END)
	return false;
    }
  return false;
  */
}
	  
static void
precedence_level1 (linked_word *scan)
{
  int           pos;
  linked_word * find_end;
  linked_word * move_along;

  while (scan->next != NULL)
    {
      switch (scan->code)
	{
	case OP_CURLY:
	case NOP_BRACKET:
	case OP_BRACKET:
	case OP_PAREN:
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

  while (scan->next != NULL)
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
  while (scan->next != NULL)
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

  while (scan->next != NULL)
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
	case MINUS:
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

  while (scan->next != NULL)
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

  while (scan->next != NULL)
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

/* IT'S SO ANNOYING HOW EACH BITWISE OPERATION HAS IT'S OWN PRECEDENCE LEVEL.
   JESUS. */

void
precedence_level7 (linked_word *scan)
{
  linked_word *hold;

  while (scan->next != NULL)
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

  while (scan->next != NULL)
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

  while (scan->next != NULL)
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

  while (scan->next != NULL)
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

  while (scan->next != NULL)
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

  while (scan->next != NULL)
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
  int     position;
  char ** result;
      
  
  result = (char **) better_malloc (sizeof (char *));

  for (position = 0; list != NULL; list = list->next, position++)
    {
      if (list->code == UNKNOWN)
	result[position] = add_newlines (list->physical, list->newlines);
      else if (list->code > UNKNOWN)
	result[position] = lookup_word (list->code - COMB_ARR, list->newlines);
      /*
      switch (list->code)
	{
	case PLUS:
	  result[position] = "+";
	  break;

	case MINUS:
	  result[position] = "-";
	  break;

	case MULTIPLY:
	  result[position] = "*";
	  break;

	case DIVIDE:
	  result[position] = "/";
	  break;

	case MOD:
	  result[position] = "%";
	  break;

	case ADD_ASSIGN:
	  result[position] = "+=";
	  break;

	case SUB_ASSIGN:
	  result[position] = "-=";
	  break;

	case MULT_ASSIGN:
	  result[position] = "*=";
	  break;

	case DIV_ASSIGN:
	  result[position] = "/=";
	  break;

	case MOD_ASSIGN:
	  result[position] = "%=";
	  break;

	case AT:
	  result[position] = "@";
	  break;

	case SET:
	  result[position] = "=";
	  break;

	case DEF:
	  result[position] = "def";
	  break;

	case DEFUNC:
	  result[position] = "defunc";
	  break;

	case FUNC_RET:
	  result[position] = "$";
	  break;

	case FUNC_OBJ:
	  result[position] = "&";
	  break;

	case IN_SCOPE:
	  result[position] = ":";
	  break;

	case COMMA:
	  result[position] = ",";
	  break;
	  
	case OP_CURLY:
	  result[position] = "{";
	  break;

	case CL_CURLY:
	  result[position] = "}";
	  break;

	case OP_BRACKET:
	  result[position] = "[";
	  break;

	case NOP_BRACKET:
	  result[position] = "![";
	  break;

	case CL_BRACKET:
	  result[position] = "]";
	  break;

	case OP_PAREN:
	  result[position] = "(";
	  break;

	case CL_PAREN:
	  result[position] = ")";
	  break;

	case QUOTE:
	  result[position] = "\"";
	  break;

	case AND:
	  result[position] = "&&";//"and";
	  break;

	case OR:
	  result[position] = "||";//"or";
	  break;

	case EQ:
	  result[position] = "==";//"eq";
	  break;

	case NEQ:
	  result[position] = "!=";//"nq";
	  break;

	case LESS:
	  result[position] = "<";
	  break;

	case MORE:
	  result[position] = ">";
	  break;

	case LESS_EQ:
	  result[position] = "<=";//"leq";
	  break;

	case MORE_EQ:
	  result[position] = ">=";//"meq";
	  break;

	case SIZE:
	  result[position] = "sizeof";
	  break;

	case IF:
	  result[position] = "if";
	  break;

	case WHILE:
	  result[position] = "while";
	  break;

	case FOR:
	  result[position] = "for";
	  break;
	  
	case THEN:
	  result[position] = "then";
	  break;

	case ELSE:
	  result[position] = "else";
	  break;
	  
	case SEMI:
	  result[position] = ";";
	  break;

	case RETURN_STAT:
	  result[position] = "return";
	  break;

	case UNKNOWN:
	  result[position] = list->physical;
	  break;

	default:
	  break;
	}
      */

      result = better_realloc (result, sizeof (char **) * (position + 1));
    }

  return result;
}

int
get_exp_length (char **words, int block)
{
  int lines;
  int pos;

  for (pos = 0; words[pos] != NULL
         && words[pos][0] != block; pos++)
    {
      for (lines = 0; words[pos][lines] == '\n'; lines++);
      if (words[pos][lines] == '\0')
	break;
      switch (words[pos][lines])
        {
        case '(':
          pos += get_exp_length (words + pos + 1, ')');
          break;
	  
	case '{':
        case '[':
          pos += get_exp_length (words + pos + 1, words[pos][0] + 2);
          break;
	  
        default:
          break;
        }
    }

  return (words[pos] == NULL) ? pos : pos + 1;
}

int
get_exp_length_first (char **words, int block)
{
  int lines;
  int pos;

  for (pos = 0; words[pos] != NULL
         && words[pos][0] != block; pos++)
    {
      for (lines = 0; words[pos][lines] == '\n'; lines++);
      if (words[pos][lines] == '\0')
	break;
      switch (words[pos][lines])
        {
        case '(':
          pos += get_exp_length (words + pos + 1, ')');
          break;
	  
	case '{':
	  return pos + get_exp_length (words + pos + 1, '}') + 1;
	  
        case '[':
          pos += get_exp_length (words + pos + 1, words[pos][0] + 2);
          break;
	  
        default:
          break;
        }
    }

  return (words[pos] == NULL) ? pos : pos + 1;
}
