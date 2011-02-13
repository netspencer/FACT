#include "FACT.h"

static unsigned byte bytes_in_pointer; /* The size of a pointer divided by the size of a single byte. */

/* This is a list of every bytecode instruction in every
 * category. Perhaps there are "better" or more effecient
 * ways to organize this, but I find this extremely clean.
 */
static struct
{
  const byte   category_byte    ;
  const byte   instruction_byte ;
  const char * id_token         ;
} instructions [] =
  {
    /* NOTE: these are sorted based on alphabetical order, so that
     * making it a lot possible to do bin search.
     */
    /* Conditional statement instructions */
    { STATEMENT, 0x0, "break"    },
    { STATEMENT, 0x1, "else"     },
    { STATEMENT, 0x2, "for"      },
    { STATEMENT, 0x3, "if"       },
    { STATEMENT, 0x4, "on_error" },
    { STATEMENT, 0x5, "return"   },
    { STATEMENT, 0x6, "while"    },
    /* Mathematical instructions */
    { MATH_CALL, 0x00, "!=" },
    { MATH_CALL, 0x01, "%"  },
    { MATH_CALL, 0x02, "%=" },
    { MATH_CALL, 0x03, "*"  },
    { MATH_CALL, 0x04, "*=" },
    { MATH_CALL, 0x05, "+"  },
    { MATH_CALL, 0x06, "+=" },
    { MATH_CALL, 0x07, "-"  },
    { MATH_CALL, 0x08, "-=" },
    { MATH_CALL, 0x09, "/"  },
    { MATH_CALL, 0x0a, "/=" },
    { MATH_CALL, 0x0b, "<"  },
    { MATH_CALL, 0x0c, "<=" },
    { MATH_CALL, 0x0d, "==" },
    { MATH_CALL, 0x0e, ">"  },
    { MATH_CALL, 0x0f, ">=" },
    { MATH_CALL, 0x10, "^"  },
    { MATH_CALL, 0x11, "`"  },
    { MATH_CALL, 0x12, "|"  },
    { MATH_CALL, 0x13, "~"  },
    /* Primitive instructions */
    /* { PRIMITIVE, 0x00, "!["  }, to be added later. */
    { PRIMITIVE, 0x01, "\""     },
    { PRIMITIVE, 0x02, "$"      },
    { PRIMITIVE, 0x03, "&"      },
    { PRIMITIVE, 0x04, "&&"     },
    /* { PRIMITIVE, 0x05, "("   }, to be added later. */
    { PRIMITIVE, 0x06, "--"     },
    { PRIMITIVE, 0x07, ":"      },
    { PRIMITIVE, 0x08, "="      },
    { PRIMITIVE, 0x09, "@"      },
    { PRIMITIVE, 0x0a, "NULL"   },
    { PRIMITIVE, 0x0b, "def"    },
    { PRIMITIVE, 0x0c, "defunc" },
    { PRIMITIVE, 0x0d, "lambda" },
    { PRIMITIVE, 0x0e, "this"   },
    { PRIMITIVE, 0x0f, "up"     },
    /* { PRIMITIVE, 0x10, "{"      }, to be added later. */
    { PRIMITIVE, 0x11, "||"     },
  };
static struct 
{
  int start ;
  int end   ;
} ends [] =
  {
    /* This is a list of every instruction category's (from
     * the instruction list) beginning and end. This is for
     * doing bin search. One could perhaps use the previous
     * category's end variable for a category's start, but
     * I think this makes things a bit nicer.
     */
    { 0x00, 0x06 }, /* Statements */
    { 0x07, 0x1a }, /* Math calls */
    { 0x1b, 0x29 }, /* Primitives -- Note, add three to end when all
		     * the commented primitives get implemented.
		     */
  };

inline char *
get_bcode_label (byte *byte_code)
{
  /* get_bcode_label - given a bytecode, return the id token
   * associated with that instruction, or NULL if it is not
   * valid bytecode or if it's a stop/ignore/number/string.
   */

  /* bsearch variables */
  int end   ;
  int mid   ;
  int start ;

  /* Check for validity of the code. */
  if (byte_code == NULL || byte_code[0] != BYTECODE
      || (byte_code[1] <= IGNORE || byte_code[1] >= NUMBER))
    return NULL;

  /* Do a bsearch for the token. We can't do a straight lookup
   * because some instructions may be missing, which would cause
   * errors.
   */
  end   = ends[byte_code[1] - STATEMENT].end;
  start = ends[byte_code[1] - STATEMENT].start;

  while (end >= start)
    {
      mid = start + (end - start) / 2;

      if (byte_code[2] < instructions[mid].instruction_byte)
	end = mid - 1;
      else if (byte_code[2] > instructions[mid].instruction_byte)
	start = mid + 1;
      else
	return (char *) instructions[mid].id_token;
    }

  /* If nothing was found, return NULL */
  return NULL;
}
  
void
compile_to_bytecode (char **expression)
{  
  /**
   * compile_to_bytecode - this function is incredibly simple. Given
   * an expression, loop through every token and replace it with a valid
   * bytecode instruction, as listed by the structure aptly named
   * 'instructions'. When we are done, send the expression to
   * compile_numbers.
   */
  
  /* bsearch variables */
  int end   ;
  int mid   ;
  int start ;
  /* index variables */
  unsigned int index    ;
  unsigned int jndex    ;
  unsigned int newlines ;
  /* static variables */
  static   int SET_SIZE ;

  /* If SET_SIZE is not set, then set it. */
  if (!SET_SIZE)
    SET_SIZE = (sizeof (ends) / sizeof (ends[0]));

  for (index = 0; expression[index] != NULL; index++)
    {
      /* Get the number of newlines in the token. */
      for (newlines = 0; expression[index][newlines] == '\n'; newlines++)
	;
      /* Skip all strings. */
      if (expression[index][newlines] == '"')
	{
	  index++;
	  continue;
	}
      /* Sweep through every category using binary search to find if
       * the current token has a valid bytecode replacement. 
       */
      for (jndex = 0; jndex < SET_SIZE; jndex++)
	{
	  start = ends[jndex].start;
	  end   = ends[jndex].end;

	  do
	    {
	      int relation; /* This is the value of the first strcmp, so
			     * we don't have three really annoying
			     * expressions that no one can understand.
			     */
	      mid = start + (end - start) / 2;
	      relation = strcmp (expression[index] + newlines,  instructions[mid].id_token);
	      
	      if (relation < 0)
		end = mid - 1;
	      else if (relation > 0)
		start = mid + 1;
	      else /* Relation can not be accessed in the while conditional. */
		break;
	    } while (end >= start);
	  if (end >= start)
	    {
	      /* Set the instruction. */

	      /* If newlines is not zero, that means that at one
	       * point we allocated memory for the token instead
	       * of using read-only memory. In this case, we use
	       * realloc as to be more conservative with memory
	       * usage.
	       */
	      if (newlines)
		expression[index] = better_realloc (expression[index], sizeof (byte) * (3 + newlines));
	      else
		expression[index] = better_malloc (sizeof (byte) * 3);

	      /* This is where we actually set the instruction.
	       * Since we used realloc, the newlines are retained
	       * and do not need to be set.
	       */
	      expression[index][newlines] = BYTECODE;
	      expression[index][newlines + 1] = instructions[mid].category_byte;
	      expression[index][newlines + 2] = instructions[mid].instruction_byte;
	      break;
	    }	
	}
    }

  /* Compile any numbers. */
  compile_constants (expression);
}

void
compile_constants (char **expression)
{
  /**
   * compile_constants - convert all numbers and strings (or other unchangeable
   * constants) to bytecode. Basically, it searches the expression for a token
   * that is a valid numerical value, or a string token ("). Then, it proceeds to
   * check if that number or string is already in our list of compiled constants.
   * If not, it converts the number to a (locked) var_t, and adds it to the list.
   * The instruction produced is essentially a var_t * spread out over 4-8 bytes,
   * depending on the architecture.
   */

  bool                   is_string    ;   // Whether or not it is a string.
  FACT_t                 returned     ;   // For the value returned by num_to_var.
  unsigned int           index        ;   // Index variables.
  unsigned int           jndex        ;
  unsigned int           newlines     ;   // Number of newlines to skip.
  unsigned long          hold_pointer ;   // Holds the pointer in a ulong type
  /* Static variables */
  static var_t        ** values         ; // Pre-compiled numbers.
  static unsigned int    next_available ; // Size of the values array.

  /* If bytes_in_pointer is not yet set, set it. */
  if (!bytes_in_pointer)
    bytes_in_pointer = (sizeof (var_t *) / sizeof (byte));

  is_string = false;
  for (index = 0; expression[index] != NULL; index++)
    {
      /* Skips all newlines. */
      for (newlines = 0; expression[index][newlines] == '\n'; newlines++);

      /* Skip all strings and valid bytecode tokens. */  
      if (expression[index][newlines] == BYTECODE)
	continue;
      if (expression[index][newlines] == '"')
	{
          expression[index] = (newlines)
            ? better_realloc (expression[index], sizeof (byte) * (newlines + 2))
            : better_malloc (sizeof (byte) * 2);
          expression[index][newlines] = BYTECODE;
          expression[index][newlines + 1] = IGNORE;
          
          if (is_string = !is_string)
            index++;
          else
            continue;
	}
      if (is_string || isnum (expression[index] + newlines))
	{
          if (is_string)
            {
              if (strlen (expression[index]) > 1)
                {
                  returned.v_point = alloc_var ();
                  returned.v_point->array_up = string_to_array (expression[index], NULL);
                  mpz_set_si (returned.v_point->array_size, strlen (expression[index]));
                }
              else
                returned.v_point = string_to_array (expression[index], NULL);
            }
          else
            returned = num_to_var (expression[index] + newlines);

          /* Loop through all the variables and see if the
	   * value has already been entered. I'm not going to
	   * go through the trouble of implementing qsort and
	   * bsearch here, so it might be a little slow for
	   * the time being.
	   */
	  for (jndex = 0; jndex < next_available; jndex++)
	    {
	      if (compare_var_arrays (values[jndex], returned.v_point, true))
		break;
	    }
	  /* If jndex equals next_available, that means the
	   * value is not already in the list, so we add it.
	   */
	  if (jndex == next_available)
	    {
	      next_available++;
	      values = better_realloc (values, sizeof (var_t *) * next_available);
	      values[jndex] = returned.v_point;
	      /* We lock the variable so it can not be edited, which would
	       * mess things up a fair bit.
	       */
	      values[jndex]->locked = true;
	    }
	  /* Set the bytecode instruction. */

	  /* We do the same thing here with newlines that we do in
	   * compile_to_bytecode. For an explanation for the realloc,
	   * read the comment there.
	   */
          expression[index] = (newlines)
            ? better_realloc (expression[index], sizeof (byte) * (2 + bytes_in_pointer + newlines))
            : better_malloc (sizeof (byte) * (2 + bytes_in_pointer));
          
	  hold_pointer = (unsigned long) values[jndex];

	  /* Set the instruction's category. */
	  expression[index][newlines] = BYTECODE;
	  expression[index][newlines + 1] = NUMBER;

	  /* This is a little bit complicated. I hope I already explained
	   * it. If not, I'll do it later.
	   */
	  newlines++;
	  for (jndex = 1; jndex <= bytes_in_pointer; jndex++)
	    {
#if (BYTE_ORDER == LITTLE_ENDIAN)
	      expression[index][jndex + newlines] = (char) (hold_pointer >> (bytes_in_pointer - jndex) * 8);
#else /* We assume that there is only little and big endian here. */
	      expression[index][jndex + newlines] = (char) (hold_pointer << (bytes_in_pointer - jndex) * 8);
#endif
	    }
	}
    }
}
