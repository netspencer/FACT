#include "FACT.h"

/* This is a list of every bytecode instruction in every
 * category. Perhaps there are "better" or more effecient
 * ways to organize this, but I find this extremely clean.
 */
static struct
{
  const byte category_byte;
  const byte instruction_byte;
  const char *id_token;
} instructions [] =
  {
    //////////////////////////
    // Primitive statements:
    //////////////////////////
    
    { STATEMENT, 0x0, "break"  },
    { STATEMENT, 0x1, "else"   },
    { STATEMENT, 0x2, "error"  },
    { STATEMENT, 0x3, "for"    },
    { STATEMENT, 0x4, "give"   },
    { STATEMENT, 0x5, "if"     },
    { STATEMENT, 0x6, "return" },
    { STATEMENT, 0x7, "while"  },

    ////////////////
    // Math calls:
    ////////////////
    
    { MATH_CALL, 0x00, "!=" },
    { MATH_CALL, 0x01, "%"  },
    { MATH_CALL, 0x02, "%=" },
    { MATH_CALL, 0x11, "&!" }, // Out of order, will be fixed.
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
    { MATH_CALL, 0x12, "|"  },
    { MATH_CALL, 0x13, "~"  },

    /////////////////////
    // Primitive calls:
    /////////////////////

    // { PRIMITIVE, 0x00, "!["  }, to be added later.
    { PRIMITIVE, 0x01, "\""     },
    { PRIMITIVE, 0x02, "$"      },
    { PRIMITIVE, 0x03, "&"      },
    { PRIMITIVE, 0x04, "&&"     },
    // { PRIMITIVE, 0x05, "("   }, to be added later. 
    { PRIMITIVE, 0x06, "--"     },
    { PRIMITIVE, 0x07, ":"      },
    { PRIMITIVE, 0x08, "="      },
    { PRIMITIVE, 0x09, "@"      },
    { PRIMITIVE, 0x0a, "NULL"   },
    { PRIMITIVE, 0x0b, "def"    },
    { PRIMITIVE, 0x0c, "defunc" },
    { PRIMITIVE, 0x0d, "lambda" },
    { PRIMITIVE, 0x0e, "sprout" },
    { PRIMITIVE, 0x0f, "this"   },
    { PRIMITIVE, 0x10, "up"     },
    // { PRIMITIVE, 0x11, "{"   }, to be added later.
    { PRIMITIVE, 0x12, "||"     },
    { PRIMITIVE, 0x13, "~!"     }, 
  };
static struct 
{
  int start;
  int end;
} ends [] =
  {
    /* This is a list of every instruction category's (from
     * the instruction list) beginning and end. This is for
     * doing bin search. One could perhaps use the previous
     * category's end variable for a category's start, but
     * I think this makes things a bit nicer.
     */
    { 0x00, 0x07 }, // Statements
    { 0x08, 0x1b }, // Math calls
    { 0x1c, 0x2c }, // Primitives
  };

inline char *
get_bcode_label (byte *byte_code)
{
  /* get_bcode_label - given a bytecode, return the id token
   * associated with that instruction, or NULL if it is not
   * valid bytecode or if it's a stop/ignore/number/string.
   */

  // bsearch variables
  int end;
  int mid;
  int start;

  // Check for validity of the code
  if (byte_code == NULL || byte_code[0] != BYTECODE
      || (byte_code[1] <= IGNORE || byte_code[1] >= CONSTANT))
    return NULL;

  /* Do a bsearch for the token. We can't do a straight lookup
   * because some instructions may be missing, which would cause
   * errors.
   */
  end = ends[byte_code[1] - STATEMENT].end;
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

#define SET_SIZE (sizeof (ends) / sizeof (ends[0]))
  
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
  int end;      
  int mid;
  int start;  
  unsigned int i, j;  
  unsigned int newlines;

  for (i = 0; expression[i] != NULL; i++)
    {
      for (newlines = 0; expression[i][newlines] == '\n'; newlines++); // Get the number of newlines in the token.

      if (expression[i][newlines] == '"')
	{
          // Skip all strings.
	  i += 2;
	  continue;
	}
      
      /* Sweep through every category using binary search to find if
       * the current token has a valid bytecode replacement. 
       */
      for (j = 0; j < SET_SIZE; j++)
	{
          end   = ends[j].end;
	  start = ends[j].start;

	  do
	    {
	      int relation;
              
	      mid = start + (end - start) / 2;
	      relation = strcmp (expression[i] + newlines, instructions[mid].id_token);
	      
	      if (relation < 0)
		end = mid - 1;
	      else if (relation > 0)
		start = mid + 1;
	      else
		break;
	    } while (end >= start);

          if (end >= start)
	    {
	      //  Set the instruction. 
              expression[i] = better_malloc (sizeof (byte) * 3);

	      /* This is where we actually set the instruction.
	       * Since we used realloc, the newlines are retained
	       * and do not need to be set.
	       */
	      expression[i][newlines] = BYTECODE;
	      expression[i][newlines + 1] = instructions[mid].category_byte;
	      expression[i][newlines + 2] = instructions[mid].instruction_byte;
	      break;
	    }	
	}
    }

  compile_constants (expression); // Compile any numbers or strings.
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

  bool          is_string;            // Whether or not it is a string.
  FACT_t        returned;             // For the value returned by num_to_var.
  unsigned int  i, j;                 // Index variables.
  unsigned int  newlines;             // Number of newlines to skip.
  unsigned long hold_pointer;         // Holds the pointer in a ulong type

  char *formatted;                    // C string with the escape characters formatted. 

  static unsigned int next_available; // Size of the values array.
  static var_t **values;              // Pre-compiled numbers.

  is_string = false;
  for (i = 0; expression[i] != NULL; i++)
    {
      for (newlines = 0; expression[i][newlines] == '\n'; newlines++); // Skips all newlines.

      // Skip all strings and valid bytecode tokens.
      if (expression[i][newlines] == BYTECODE)
	continue;
      if (expression[i][newlines] == '"')
	{
          expression[i] = better_malloc (sizeof (byte) * 2);
          expression[i][0] = BYTECODE;
          expression[i][0 + 1] = IGNORE;
          
          if (is_string = !is_string)
            i++;
          else
            continue;
	}
      if (is_string || isnum (expression[i] + newlines))
	{
          if (is_string)
            {
              formatted = rm_cslashes (expression[i]);
              if (strlen (formatted) > 1)
                {
                  returned.v_point = alloc_var ();
                  returned.v_point->array_up = string_to_array (formatted, NULL);
                  mpz_set_si (returned.v_point->array_size, strlen (formatted));
                }
              else
                returned.v_point = string_to_array (formatted, NULL);
            }
          else
            returned = num_to_var (expression[i] + newlines);

          /* Loop through all the variables and see if the
	   * value has already been entered. I'm not going to
	   * go through the trouble of implementing qsort and
	   * bsearch here, so it might be a little slow for
	   * the time being.
	   */
	  for (j = 0; j < next_available; j++)
	    {
	      if (compare_var_arrays (values[j], returned.v_point, true) == 0)
		break;
	    }
	  /* If jndex equals next_available, that means the
	   * value is not already in the list, so we add it.
	   */
	  if (j == next_available)
	    {
	      next_available++;
	      values = better_realloc (values, sizeof (var_t *) * next_available);
	      values[j] = returned.v_point;
	      /* We lock the variable so it can not be edited, which would
	       * mess things up a fair bit.
	       */
	      values[j]->locked = true;
	    }
	  /* Set the bytecode instruction. */

	  /* We do the same thing here with newlines that we do in
	   * compile_to_bytecode. For an explanation for the realloc,
	   * read the comment there.
	   */
          expression[i] = FACT_malloc (sizeof (byte) * (2 + BYTES_IN_POINTER));
          
	  hold_pointer = (unsigned long) values[j];

	  // Set the instruction's category. 
	  expression[i][0] = BYTECODE;
	  expression[i][1] = CONSTANT;

	  /* This is a little bit complicated. I hope I already explained
	   * it. If not, I'll do it later.
	   */
	  for (j = 0; j < BYTES_IN_POINTER; j++)
	    {
#if (BYTE_ORDER == LITTLE_ENDIAN)
	      expression[i][j + 2] = (char) (hold_pointer >> (BYTES_IN_POINTER - (j + 1)) * 8);
#else
	      expression[i][j + 2] = (char) (hold_pointer << (BYTES_IN_POINTER - (j + 1)) * 8);
#endif
	    }
	}
    }
}
