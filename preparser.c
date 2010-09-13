#include "preparser.h"

char get_block_code (char *block)
{
  if (!strcmp (block, "+"))
    return PLUS;
  else if (!strcmp (block, "-"))
    return MINUS;
  else if (!strcmp (block, "*"))
    return MULTIPLY;
  else if (!strcmp (block, "/"))
    return DIVIDE;
  else if (!strcmp (block, "%"))
    return MOD;
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
  else if (!strcmp (block, "{"))
    return OP_CURLY;
  else if (!strcmp (block, "}"))
    return CL_CURLY;
  else if (!strcmp (block, "["))
    return OP_BRACKET;
  else if (!strcmp (block, "]"))
    return CL_BRACKET;
  else if (!strcmp (block, "("))
    return OP_PAREN;
  else if (!strcmp (block, ")"))
    return CL_PAREN;
  else if (!strcmp (block, "\""))
    return QUOTE;
  else if (!strcmp (block, "and"))
    return AND;
  else if (!strcmp (block, "or"))
    return OR;
  else if (!strcmp (block, "eq"))
    return EQ;
  else if (!strcmp (block, "nq"))
    return NEQ;
  else if (!strcmp (block, "<"))
    return LESS;
  else if (!strcmp (block, ">"))
    return MORE;
  else if (!strcmp (block, "leq"))
    return LESS_EQ;
  else if (!strcmp (block, "meq"))
    return MORE_EQ;
  else if (!strcmp (block, "sizeof"))
    return SIZE;
  else if (!strcmp (block, "if"))
    return IF;
  else if (!strcmp (block, "while"))
    return WHILE;
  else if (!strcmp (block, ";"))
    return SEMI;
  else if (!strcmp (block, "return"))
    return RETURN;
  else
    return UNKNOWN;
}
  
static inline linked_word *alloc_word ()
{
  linked_word *temp;

  temp = (linked_word *) better_malloc (sizeof (linked_word));

  temp->code = UNKNOWN;
  temp->physical = NULL;
  temp->hidden = NULL;
  temp->next = NULL;
  temp->previous = NULL;

  return temp;
}

linked_word *create_links (char **scan, char end)
{
  char w_code;
  int move_length;
  int pos;
  linked_word *current;

  for (current = alloc_word (); *scan != NULL
	 && get_block_code (*scan) != end; scan++)
    {
      switch ((w_code = get_block_code (*scan)))
	{
	case UNKNOWN:
	  current->physical = *scan;
	  break;
	case OP_CURLY:
	  current->hidden = create_links (++scan, CL_CURLY);
	  
	  for (move_length = 0; scan[move_length] != NULL
		 && get_block_code (scan[move_length]) != CL_CURLY; move_length++);

	  current->code = w_code;
	  current->next = create_links (scan + (++move_length), end);
	  current->next->previous = current;

	  while (current->previous != NULL)
	    current = current->previous;

	  return current;
	case OP_BRACKET:
	  current->hidden = create_links (++scan, CL_BRACKET);
	  
	  for (move_length = 0; scan[move_length] != NULL
		 && get_block_code (scan[move_length]) != CL_BRACKET; move_length++);

	  printf ("-> %s\n", scan[move_length + 1]);

	  move_length++;
	  current->code = OP_BRACKET;
	  current->next = create_links (scan + (++move_length), end);
	  current->next->previous = current;

	  while (current->previous != NULL)
	    current = current->previous;

	  return current;
	case OP_PAREN:
	  current->hidden = create_links (++scan, CL_PAREN);
	  
	  for (move_length = 0; scan[move_length] != NULL
		 && get_block_code (scan[move_length]) != CL_PAREN; move_length++);

	  current->code = w_code;
	  current->next = create_links (scan + (++move_length), end);
	  current->next->previous = current;

	  while (current->previous != NULL)
	    current = current->previous;

	  return current;
	case DEF:
	case DEFUNC:
	  current->hidden = create_links (++scan, UNKNOWN);
	  
	  for (move_length = 0; scan[move_length] != NULL
		 && get_block_code (scan[move_length]) != UNKNOWN; move_length++);

	  current->code = w_code;
	  current->next = create_links (scan + (++move_length), end);
	  current->next->previous = current;

	  while (current->previous != NULL)
	    current = current->previous;

	  return current;
	default:
	  break;
	}

      current->code = w_code;
      current->next = alloc_word ();
      current->next->previous = current;
      current = current->next;
    }

  current->code = end;
  current->physical = (end == UNKNOWN) ? *scan : NULL;

  while (current->previous != NULL)
    current = current->previous;

  return current;
}
