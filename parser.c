#include "FACT.h"

/////////////////////
// Lexer functions:
/////////////////////

char *
add_newlines (char *word, int newlines)
{
  /**
   * add_newlines - this function, given a string and a value,
   * returns a string allocated in memory of the passed string
   * combined with the value in newlines at the begining.
   * For example: word = "hello, world!", newlines = "2" would
   * return "\n\nhello, world!"
   */
  int i;
  int word_len;

  char *new_str;

  if (word[0] == BYTECODE)
    {
      if (word[1] == CONSTANT)
        word_len = (sizeof (var_t *) / sizeof (char)) + 2;
      else
        word_len = 3;
    }
  else
    word_len = strlen (word);

  new_str = better_malloc (sizeof (char) * (word_len + newlines + 1));

  for (i = 0; i < newlines; i++)
    new_str[i] = '\n';

  while (i < word_len + newlines)
    new_str[i++] = word[i - newlines];
  new_str[i] = '\0';

  return new_str;
}

static bool
is_in_quotes (int character)
{
  static bool prev_slash;
  static bool is_quote;

  /**
   * is_in_quotes - given a character, check whether or not it is
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
is_opt (int op1, int op2)
{
  /**
   * isopt - returns true when passed characters of the
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
      || (op1 == '|' && op2 == '|') || (op1 == '-' && op2 == '>'))
    return true;
  return false;
}

char **
get_words (char *start)
{
  int i, j;
  bool is_string;

  char *end;
  char **result;

  /**
   * get_words - the main tokenizing routine. I wrote this code
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

  for (i = 0, end = start, is_string = false, result = NULL;
       *end != '\0'; i++)
    {
      result = better_realloc (result, (i + 1) * sizeof (char *));

      while (isspace ((int) *end) && !is_in_quotes ((int) *end)
             && *end != '\n' && *end != '\0')
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
      else if (is_opt ((int) *end, (int) *(end + 1)))
        end += 2;
      else if (ispunct ((int) *end) && *end != '.')
        end++;
      else
        {
          while (isalnum (*end) || *end == '_'
                 || *end == '.')
            end++;
        }

      if ((end - start) > 0)
        {
          result[i] = better_malloc ((end - start + 1) * sizeof (char));

          for (j = 0; start != end; start++, j++)
            result[i][j] = *start;

          result[i][j] = '\0';
          start = end;
        }
    }

  result[i - 1] = NULL;
  return (i > 0) ? result : NULL;
}

/////////////////////
// Stack functions:
/////////////////////

static void *
pop_s (void **stack, int *stack_size)
{
  void *ret;

  if (*stack_size == 0)
    return NULL;
  else
    {
      (*stack_size)--;
      ret = stack[*stack_size];
      stack[*stack_size] = NULL;
    }
  return ret;
}

static inline void
push_s (void **stack, int *stack_size, void *val)
{
  stack[*stack_size] = val;
  (*stack_size)++;
}

///////////////////////
// Parsing functions: 
///////////////////////

int
op_get_prec (char *op)
{
  int i, j;
  // We assume 20 is the max number of tokens per level.
  static char *prec_table[][20] =
    {
      // Operators: 0 -> 14
      { "def", "defunc", NULL },
      { "$", "&", NULL },
      { ":", NULL },
      { "@", NULL },
      { "--", NULL },
      { "~", "*", "/", "%", NULL },
      { "+", "-", NULL },
      { "<", "<=", ">", ">=", NULL },
      { "==", "!=", NULL },
      { "`", NULL },
      { "^", NULL },
      { "|", NULL },
      { "&&", NULL },
      { "||", NULL },
      { "=", "+=", "-=", "*=", "/=", "%=", NULL },
      // Special keywords (highest precedence): 15 -> 18
      { "if", "error", "while", "for", "then", "else", NULL },
      { "return", "give", "break", "sprout", ";", ",", ")!", NULL },
      { "![", "[", "(", "{", NULL },
      { "]", ")", "}", NULL },
    };

  for (i = 0; i < (sizeof (prec_table) / sizeof (prec_table[0])); i++)
    {
      for (j = 0; prec_table[i][j] != NULL; j++)
        {
          if (!strcmp (prec_table[i][j], op))
            return i;
        }
    }
  return -1;
}

static inline bool
op_is_lr (int prec)
{
  return (prec == 14)
    ? false
    : true;
}

////////////////////
// Error checking:
////////////////////

bool
check (char **input, const char *f_name, int start_line)
{
  int i, j;
  int prec;
  int state;
  int s_size;
  int p_count;

  char *custom_fmt;
  char *next_token;
  char *prev_token;
  bool *com_stack;
  bool *def_stack;
  bool *semi_stack;

#define STACK_REALLOC() com_stack = FACT_realloc (com_stack, sizeof (bool) * (s_size + 1)); \
  def_stack = FACT_realloc (def_stack, sizeof (bool) * (s_size + 1));   \
  semi_stack = FACT_realloc (semi_stack, sizeof (bool) * (s_size + 1))

  // Possible values of state
#define START 0
#define OP    1
#define VAR   2

  com_stack = FACT_malloc (sizeof (bool));
  def_stack = FACT_malloc (sizeof (bool));
  semi_stack = FACT_malloc (sizeof (bool));

  prev_token = NULL;
  state = START;

  s_size = 1;
  semi_stack[s_size - 1] = true;
  com_stack[s_size - 1] = def_stack[s_size - 1] = false;

  for (i = 0; input[i] != NULL; i++)
    {
      if (input[i][0] == '\n')
        continue;
      
      next_token = input[i + 1];
      if (def_stack[s_size - 1])
        {
          if (op_get_prec (input[i]) == -1)
            {
              s_size--;
              STACK_REALLOC ();
              com_stack[s_size] = def_stack[s_size] = semi_stack[s_size] = false;
              state = VAR;
              continue;
            }
          if (strcmp (input[i], "["))
            goto error;
        }
          
      if (!strcmp (input[i], "("))
        {
          if (prev_token != NULL && (!strcmp (prev_token, "$")
                                     || !strcmp (prev_token, "&")))
            {
              state = OP;
              STACK_REALLOC ();
              com_stack[s_size] = true;
              def_stack[s_size] = semi_stack[s_size] = false;
              s_size++;
            }
          else
            {
              if (state == VAR && (prev_token != NULL && strcmp (prev_token, "}")))
                goto error;
              state = OP;
              STACK_REALLOC ();
              com_stack[s_size] = def_stack[s_size] = semi_stack[s_size] = false;
              s_size++;
            }
        }
      else if (input[i][0] == ')')
        {
          s_size--;
          STACK_REALLOC ();
          com_stack[s_size] = def_stack[s_size] = semi_stack[s_size] = false;
          if ((input[i][1] != ')' && state == OP)
              || (state != VAR && (prev_token == NULL || strcmp (prev_token, "("))))
            goto error;
          if (input[i][1] != '\0')
            {
              state = START;
              if (input[i][1] == ')')
                input[i] = ")";
            }
        }
      else if (!strcmp (input[i], "["))
        {
          if (state != VAR)
            {
              STACK_REALLOC ();
              com_stack[s_size] = true;
              def_stack[s_size] = semi_stack[s_size] = false;
              s_size++;
              input[i] = "![";
            }
          else
            {
              STACK_REALLOC ();
              com_stack[s_size] = def_stack[s_size] = semi_stack[s_size] = false;
              s_size++;
            }
          state = OP;
        }
      else if (!strcmp (input[i], "]"))
        {
          if (state != VAR)
            goto error;
          s_size--;
          STACK_REALLOC ();
          com_stack[s_size] = def_stack[s_size] = semi_stack[s_size] = false;
        }
      else if (!strcmp (input[i], "{"))
        {
          if (state == VAR && (prev_token != NULL && strcmp (prev_token, "}")))
            goto error;
          state = START;
          STACK_REALLOC ();
          semi_stack[s_size] = true;
          com_stack[s_size] = def_stack[s_size] = false;
          s_size++;
        }
      else if (!strcmp (input[i], "}"))
        {
          if (strcmp (prev_token, ";") && strcmp (prev_token, "}"))
            {
              custom_fmt = "expected ';' before %s";
              goto custom_error;
            }
          s_size--;
          STACK_REALLOC ();
          com_stack[s_size] = def_stack[s_size] = semi_stack[s_size] = false;
          state = VAR;
        }
      else if (!strcmp (input[i], "$") || !strcmp (input[i], "&"))
        {
          if (state == VAR && (prev_token != NULL && strcmp (prev_token, "}")))
            goto error;
          if (strcmp (next_token, "("))
            {
              custom_fmt = "expected '(' after %s";
              goto custom_error;
            }
        }
      else if (!strcmp (input[i], ","))
        {
          if (!com_stack[s_size - 1] || state != VAR)
            goto error;
          state = OP;
        }
      else if (!strcmp (input[i], ";"))
        {
          if (!semi_stack[s_size - 1] || state != VAR)
            goto error;
          state = START;
        }
      else if (!strcmp (input[i], "@"))
        {
          if (state != VAR || next_token == NULL || strcmp (next_token, "("))
            goto error;
          for (j = i + 2, p_count = 1; p_count > 0; j++)
            {
              if (!strcmp (input[j], "("))
                p_count++;
              else if (!strcmp (input[j], ")"))
                p_count--;
            }
          if (input[j] == NULL || strcmp (input[j], "{"))
            {
              custom_fmt = "expected '{' after %s";
              goto custom_error;
            }
          input[j - 1] = "))";
          i++;
          state = OP;
          STACK_REALLOC ();
          com_stack[s_size] = true;
          def_stack[s_size] = semi_stack[s_size] = false;
          s_size++;
        }
      else if (!strcmp (input[i], "if") || !strcmp (input[i], "while")
               || !strcmp (input[i], "error"))
        {
          // Todo: check for correct/incorrect break placements in while.
          if (state != START && (prev_token != NULL && strcmp (prev_token, "}")))
            goto error;
          if (next_token == NULL || strcmp (next_token, "("))
            {
              custom_fmt = "expected '(' after %s";
              goto custom_error;
            }
          for (j = i + 2, p_count = 1; p_count > 0; j++)
            {
              if (!strcmp (input[j], "("))
                p_count++;
              else if (!strcmp (input[j], ")"))
                p_count--;
            }
          input[j - 1] = ")!";
          state = OP;
        }
      else if (!strcmp (input[i], "for"))
        {
          if (state != START && (prev_token != NULL && strcmp (prev_token, "}")))
            goto error;
          STACK_REALLOC ();
          com_stack[s_size] = true;
          def_stack[s_size] = semi_stack[s_size] = false;
          s_size++;
          state = OP;
        }
      else if (!strcmp (input[i], "then"))
        {
          if (state != VAR)
            goto error;
          s_size--;
          STACK_REALLOC ();
          com_stack[s_size] = def_stack[s_size] = semi_stack[s_size] = false;
          state = START;
        }
      else if (!strcmp (input[i], "def") || !strcmp (input[i], "defunc"))
        {
          if (state == VAR && (prev_token != NULL && strcmp (prev_token, "}")))
            goto error;
          state = VAR;
          STACK_REALLOC ();
          def_stack[s_size] = true;
          com_stack[s_size] = semi_stack[s_size] = false;
          s_size++;
        }
      else if (!strcmp (input[i], "sprout"))
        {
          if (state == VAR && (prev_token != NULL && strcmp (prev_token, "}")))
            goto error;
          state = START;
        }
      else if (!strcmp (input[i], "return") || !strcmp (input[i], "give"))
        {
          if (state != START && (prev_token != NULL && strcmp (prev_token, "}")))
            goto error;
          if (next_token == NULL || !strcmp (next_token, ";"))
            {
              custom_fmt = "%s cannot be alone in an expression.";
              goto custom_error;
            }
          state = OP;
        }
      else if (!strcmp (input[i], "else"))
        {
          if (state != START && (prev_token != NULL && strcmp (prev_token, "}")))
            goto error;
          state = START;
        }
      else if ((prec = op_get_prec (input[i])) <= 14 && prec >= 1 && prec != 3)
        {
          // If we've hit an operator
          if (state != VAR)
            {
              if (prev_token != NULL && strcmp (prev_token, "--"))
                input[i] = "--";
              else
                goto error;
            }
          state = OP;
        }
      else if (!strcmp (input[i], "\""))
        {
          // Skip quotes.
          if (state == VAR && (prev_token != NULL && strcmp (prev_token, "}")))
            goto error;
          state = VAR;
          i += 2;
        }
      else
        {
          // We've hit a variable.
          if (state == VAR && (prev_token != NULL && strcmp (prev_token, "}")))
            goto error;
          state = VAR;
        }          
      prev_token = input[i];
    }

  FACT_free (com_stack);
  FACT_free (def_stack);
  FACT_free (semi_stack);
  return false;

 error:
  custom_fmt = "unexpected %s";
 custom_error:
  fprintf (stderr, "E> Parsing error in (%s) on line %d: ",  f_name, start_line);
  fprintf (stderr, custom_fmt, input[i]);
  fprintf (stderr, "\n");
  FACT_free (com_stack);
  FACT_free (def_stack);
  FACT_free (semi_stack);
  return true;
}

char **
parse (char **input)
{
  /**
   * parse - convert the input string array from infix to polish
   * notation. We assume that input has already been checked for
   * errors. Algorithm used is shunting yard.
   */
  int i, j;
  int p_count;
  int stack_size;
  char *goal;
  char *hold;
  char **op_stack;
  char **output;

  stack_size = 0;

  // TEMP: remove newlines
  for (i = 0; input[i] != NULL; i++)
    {
      if (input[i][0] == '\n')
        {
          for (j = i; input[j] != NULL; j++)
            input[j] = input[j + 1];
        }
    }
  
  // Get the length and allocate space for output.
  for (i = 0; input[i] != NULL; i++);
  output = FACT_malloc (sizeof (char *) * (i + 1));
  op_stack = FACT_malloc (sizeof (char *) * (i + 1));

  // TEMP: ignore results
  check (input, "test", 0);

  for (i--, j = 0; i >= 0; i--)
    {
      if (!strcmp (input[i], "\""))
        {
          output[j++] = "\"";
          output[j++] = input[--i];
          output[j++] = "\"";
          i--;
          continue;
        }
      switch (op_get_prec (input[i]))
        {
        case -1:
          // If the current token is a number or variable.
          output[j++] = input[i];
          break;

        case 15:
        case 16:
          /* Keep popping till the stack is empty, or until there's a ')',
           * '}', or ']'.
           */
          while (stack_size > 0)
            {
              hold = op_stack[stack_size - 1];
              if (!strcmp (hold, ")")
                  || !strcmp (hold, "}")
                  || !strcmp (hold, "]"))
                break;
              output[j++] = (char *) pop_s ((void **) op_stack, &stack_size);
            }
          if (!strcmp (input[i], ")!"))
            {
              input[i] = output[j++] = ")";
              push_s ((void **) op_stack, &stack_size, ")");
            }
          else
            output[j++] = input[i];
          break;

        case 17:
          if (!strcmp ("![", input[i]) || !strcmp ("[", input[i]))
            goal = "]";
          else if (!strcmp ("(", input[i]))
            goal = ")";
          else // "{"
            goal = "}";

          while (strcmp ((hold = (char *) pop_s ((void **) op_stack, &stack_size)), goal)) 
            output[j++] = hold;
          output[j++] = input[i];
          break;

        case 18:
          if (!strcmp (input[i], "}"))
            {
              while (stack_size > 0)
                {
                  hold = op_stack[stack_size - 1];
                  if (!strcmp (hold, ")")
                      || !strcmp (hold, "}")
                      || !strcmp (hold, "]"))
                    break;
                  output[j++] = (char *) pop_s ((void **) op_stack, &stack_size);
                }
            }
          push_s ((void **) op_stack, &stack_size, input[i]);
          output[j++] = input[i];
          break;

        default:
          // Every other value.
          while (stack_size > 0)
            {
              // Coded without any prior though (banged out).
              int op1_prec;
              int op2_prec;

              hold = op_stack[stack_size - 1];
              op1_prec = op_get_prec (input[i]);
              op2_prec = op_get_prec (hold);
              
              if (op_is_lr (op1_prec))
                {
                  if (op1_prec >= op2_prec)
                    output[j++] = (char *) pop_s ((void **) op_stack, &stack_size);
                  else
                    break;
                }
              else if (op1_prec > op2_prec)
                output[j++] = (char *) pop_s ((void **) op_stack, &stack_size);
              else
                break;
            }
          push_s ((void **) op_stack, &stack_size, input[i]);
          break;
        }
    }

  // Pop the rest of the stack.
  while (stack_size > 0)
    output[j++] = (char *) pop_s ((void **) op_stack, &stack_size);
  output[j] = NULL;
  
  // Reverse the output array.
  for (i = 0, j--; j > i; i++, j--)
    {
      hold = output[j];
      output[j] = output[i];
      output[i] = hold;
    }

  // For debug purposes
  printf ("Parsing results: \n");
  for (i = 0; output[i] != NULL; i++)
    printf (" %s", output[i]); 
  printf ("\nDone.\n");
  
  // Free up the stack and return.
  FACT_free (op_stack);

  // This will be optomized in the near future.
  compile_to_bytecode (output);
  
  return output;
}

static inline int
get_end_block (int block)
{
  switch (block)
    {
    case '(':
      return ')';
    case '[':
    case '!':
      return ']';
    case '{':
      return '}';
    default:
      return 0;	// NOTREACHED
    }
}

int
get_exp_length (char **words, int end_block)
{
  int i;
  int lines;

  for (i = 0; words[i] != NULL; i++)
    {
      for (lines = 0; words[i][lines] == '\n'; lines++);

      if (words[i][lines] == '\0' || words[i][lines] == end_block)
        break;

      if (words[i][lines] == '('
          || words[i][lines] == '['
          || (words[i][lines] == '!'
              && words[i][lines + 1] == '[')
          || words[i][lines] == '{')
        i += get_exp_length (words + i + 1,
                             get_end_block (words[i]
                                            [lines]));
    }

  return (words[i] == NULL) ? i : i + 1;
}

int
get_exp_length_first (char **words, int end_block)
{
  int i;
  int lines;

  for (i = 0; words[i] != NULL; i++)
    {
      for (lines = 0; words[i][lines] == '\n'; lines++);

      if (words[i][lines] == '\0' || words[i][lines] == end_block)
        break;

      if (words[i][lines] == '('
          || words[i][lines] == '['
          || (words[i][lines] == '!'
              && words[i][lines + 1] == '['))
        i += get_exp_length (words + i + 1,
                             get_end_block (words[i]
                                            [lines]));
      else if (words[i][lines] == '{')
        return i + get_exp_length (words + i + 1, '}') + 1;
    }

  return (words[i] == NULL) ? i : i + 1;
}
