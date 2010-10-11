/*
 * parser.c
 * (c) 2010 Matthew Plant
 * Includes functions for parsing input into
 * more easily used data.
 *
 */

#include "parser.h"

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
	{
	  times = 0;
	  return false;
	}
      return true;

    default:
      times = 0;
      return false;
    }
}

char **
get_words (char *start)
{
  int count;
  int count2;

  bool isstring;

  char *end;
  char **return_string;

  return_string = (char **) better_malloc (sizeof (char *));
  
  for (count = 0, end = start, isstring = false; *end != '\0'; count++)
    {
      return_string = (char **) better_realloc (return_string, (count + 1) * sizeof (char *));

      while (isspace ((int) *end) && !is_in_quotes ((int) *end))
	start = ++end;

      if (isstring)
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
      else if (ispunct ((int) *end))
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
	  start = end;
	}
    }
      
  return_string[count - 1] = NULL;
  
  return (count > 0) ? return_string : NULL;
}

word_code
get_block_code (char *block)
{
  if (block == NULL)
    return END;
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
  else if (!strcmp (block, "<-"))
    return FUNC_END;
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
  linked_word *temp;

  temp = (linked_word *) better_malloc (sizeof (linked_word));

  temp->code = UNKNOWN;
  temp->physical = NULL;
  temp->hidden = NULL;
  temp->hidden_up = NULL;
  temp->next = NULL;
  temp->previous = set_prev;

  return temp;
}

linked_word *
create_list (char **words)
{
  word_code w_code;
  linked_word *base;

  for (base = alloc_word (NULL); (w_code = get_block_code (words[0])) != END; words++)
    {
      if (w_code == UNKNOWN)
	base->physical = words[0];

      base->code = w_code;

      base->next = alloc_word (base);
      base = base->next;
    }

  base->code = END;

  while (base->previous != NULL)
    base = base->previous;

  return base;
}

/* Got to add a way to check for parsing errors */
linked_word *
set_list (linked_word *start, word_code stopper) /* Sets a list of linked words to the correct format */
{
  word_code w_code;
  linked_word *temp_link;
  
  while ((w_code = start->code) != stopper && w_code != END) /* While the current node is not the last node and it's not the desired node, */
    {
      if (w_code == OP_CURLY) /* If the node is a opening curly bracket */
	{
 	  /* Set the first hidden node to the next node in the list */ 
	  start->hidden = start->next;
	  /* Set the previous node in first hidden node to NULL */
	  start->next->previous = NULL;
	  /* Set temp link to a list that ends with '}' */
      	  temp_link = set_list (start->next, CL_CURLY);

	  if (temp_link->code == END)
	    return temp_link;
	  
	  start->next = temp_link->next;
	  temp_link->next->previous = start;
	  temp_link->next = NULL;

	  start->hidden->hidden_up = start;
	}
      else if (w_code == OP_BRACKET
	       || w_code == NOP_BRACKET)
	{
	  start->hidden = start->next;
	  start->next->previous = NULL;
	  temp_link = set_list (start->next, CL_BRACKET);

	  if (temp_link->code == END)
	    return temp_link;
	  
	  start->next = temp_link->next;
	  temp_link->next->previous = start;
	  temp_link->next = NULL;

	  start->hidden->hidden_up = start;
	}
      else if (w_code == OP_PAREN)
	{
	  start->hidden = start->next;
	  start->next->previous = NULL;
	  temp_link = set_list (start->next, CL_PAREN);

	  if (temp_link->code == END)
	    return temp_link;
	  
	  start->next = temp_link->next;
	  temp_link->next->previous = start;
	  temp_link->next = NULL;

	  start->hidden->hidden_up = start;
	}
      else if (w_code == FUNC_RET
	       || w_code == FUNC_OBJ)
	{
	  start->hidden = start->next;
	  start->next->previous = NULL;
	  temp_link = set_list (start->next, FUNC_END);

	  if (temp_link->code == END)
	    return temp_link;
	  
	  start->next = temp_link->next;
	  temp_link->next->previous = start;
	  temp_link->next = NULL;

	  start->hidden->hidden_up = start;
	}
      else if (w_code == DEF
	       || w_code == DEFUNC)
	{
	  start->hidden = start->next;
	  start->next->previous = NULL;
	  temp_link = set_list (start->next, UNKNOWN);

	  if (temp_link->code == END)
	    return temp_link;
	  
	  start->next = temp_link->next;
	  temp_link->next->previous = start;
	  temp_link->next = NULL;

	  start->hidden->hidden_up = start;
	}
      else if (w_code == QUOTE)
	{
	  start->hidden = start->next;
	  start->next->previous = NULL;
	  temp_link = set_list (start->next, QUOTE);

	  if (temp_link->code == END)
	    return temp_link;
	  
	  start->next = temp_link->next;
	  temp_link->next->previous = start;
	  temp_link->next = NULL;

	  start->hidden->hidden_up = start;
	}

      start = start->next;
    }
  
  return start;
}

void
swap (linked_word *swapping)
{
  linked_word *temp_next;
  linked_word *temp_prev;

  if (swapping->previous == NULL)
    return;

  if (swapping->previous->hidden_up != NULL)
    {
      swapping->hidden_up = swapping->previous->hidden_up;
      swapping->hidden_up->hidden = swapping;
      swapping->previous->hidden_up = NULL;
    }

  temp_next = swapping->next;
  temp_prev = swapping->previous->previous;

  swapping->previous->next = temp_next;
  swapping->previous->previous = swapping;

  swapping->next = swapping->previous;
  swapping->previous = temp_prev;

  if (temp_next != NULL)
    temp_next->previous = swapping->next;

  if (temp_prev != NULL)
    temp_prev->next = swapping;
}

static void
precedence_level1 (linked_word *scan)
{
  linked_word *find_end;

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
	      
	      scan->next->next = find_end;
	      find_end->previous = scan->next;
	    }
	  else
	    {
	      scan->next->previous = NULL;
	      rev_shunting_yard (scan->next->next);
	      
	      for (find_end = scan->next;
		   find_end->previous != NULL;
		   find_end = find_end->previous);
	      
	      scan->next = find_end;
	      find_end->previous = scan;
	    }
	  return;

	case FUNC_RET:
	case FUNC_OBJ:
	  if (scan->hidden->hidden != NULL)
	    rev_shunting_yard (scan->hidden->hidden);

	  scan->hidden->next->previous = NULL;
	  rev_shunting_yard (scan->hidden->next);

	  for (find_end = scan->hidden->next;
	       find_end->previous != NULL;
	       find_end = find_end->previous);
	  
	  scan->hidden->next = find_end;
	  find_end->previous = scan->hidden;
	  break;

	case FOR:
	case THEN:
	case ELSE:
	case IN_SCOPE:
	case COMMA:
	case RETURN_STAT:
	case CL_CURLY:
	case SEMI:
	  scan->next->previous = NULL;
	  rev_shunting_yard (scan->next);

	  for (find_end = scan->next;
	       find_end->previous != NULL;
	       find_end = find_end->previous);

	  scan->next = find_end;
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
  while (scan->next != NULL)
    {
      switch (scan->code)
	{
	case IF:
	case WHILE:
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
precedence_level3 (linked_word *scan)
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
	  
	case PLUS:
	case MINUS:
	  hold = scan->next;
	  while (scan->previous != NULL
		 && scan->previous->code != PLUS
		 && scan->previous->code != MINUS
		 && scan->previous->code != SET
		 && scan->previous->code != OP_CURLY
		 && (scan->previous->code < ADD_ASSIGN
		     || scan->previous->code > MOD_ASSIGN)
		 && (scan->previous->code < AND
		     || scan->previous->code > MORE_EQ))
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
precedence_level4 (linked_word *scan)
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
		 && scan->previous->code != SET
		 && scan->previous->code != OP_CURLY
		 && (scan->previous->code < ADD_ASSIGN
		     || scan->previous->code > MOD_ASSIGN)
		 && (scan->previous->code < AND
		     || scan->previous->code > MORE_EQ))
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
	  
	case EQ:
	case NEQ:
	  hold = scan->next;
	  while (scan->previous != NULL
		 && scan->previous->code != SET
		 && scan->previous->code != OP_CURLY
		 && (scan->previous->code < ADD_ASSIGN
		     || scan->previous->code > MOD_ASSIGN)
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
	  
	case AND:
	  hold = scan->next;
	  while (scan->previous != NULL
		 && scan->previous->code != SET
		 && scan->previous->code != OP_CURLY
		 && (scan->previous->code < ADD_ASSIGN
		     || scan->previous->code > MOD_ASSIGN)
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
	  
	case OR:
	  hold = scan->next;
	  while (scan->previous != NULL
		 && scan->previous->code != SET
		 && scan->previous->code != OP_CURLY
		 && (scan->previous->code < ADD_ASSIGN
		     || scan->previous->code > MOD_ASSIGN)
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
	  
	case SET:
	case ADD_ASSIGN:
	case SUB_ASSIGN:
	case MULT_ASSIGN:
	case DIV_ASSIGN:
	case MOD_ASSIGN:
	  hold = scan->next;
	  while (scan->previous != NULL
		 && scan->previous->code != SET
		 && scan->previous->code != OP_CURLY
		 && (scan->previous->code < ADD_ASSIGN
		     || scan->previous->code > MOD_ASSIGN))
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
  precedence_level1 (scan);
  precedence_level2 (scan);
  precedence_level3 (scan);
  precedence_level4 (scan);
  precedence_level5 (scan);
  precedence_level6 (scan);
  precedence_level7 (scan);
  precedence_level8 (scan);
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
	  temp_next = scan->next;
	  scan->next = scan->hidden;
	  scan->hidden->previous = scan;
	  set_end (scan->next, temp_next);
	}
       
      scan = scan->next;
    }
}  

char **
convert_link (linked_word *list)
{
  char **result;
  int position;

  result = (char **) better_malloc (sizeof (char *));

  for (position = 0; list != NULL; list = list->next, position++)
    {
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

	case FUNC_END:
	  result[position] = "<-";
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

      result = better_realloc (result, sizeof (char **) * (position + 1));
    }

  return result;
}

int
get_exp_length (char **words, int block)
{
  int pos;

  for (pos = 0; words[pos] != NULL
         && words[pos][0] != block; pos++)
    {
      switch (words[pos][0])
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
  int pos;

  for (pos = 0; words[pos] != NULL
         && words[pos][0] != block; pos++)
    {
      switch (words[pos][0])
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
