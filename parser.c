/* This file is part of FACT.
 *
 * FACT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FACT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FACT. If not, see <http://www.gnu.org/licenses/>.
 */

#include "FACT.h"

/////////////////////
// Lexer functions:
/////////////////////

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
   *  &=
   *  |=
   *  ^=
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
        || op1 == '!'
        /*
        || op1 == '&'
        || op1 == '|'
        || op1 == '^'*/) && op2 == '=')
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

      // Check for a string
      if (is_string)
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
      // Check for newlines
      else if (*end == '\n')
        {
          while (*end == '\n')
            end++;
        }
      // Check for everything else
      else if (is_opt ((int) *end, (int) *(end + 1)))
        end += 2;
      else if (ispunct ((int) *end) && *end != '.')
        end++;
      else
        {
          while (isalnum (*end) || *end == '_' || *end == '.')
            end++;
        }

      // Tokenize
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
      { "--", "~!", NULL },
      { "~", "*", "/", "%", NULL },
      { "+", "-", NULL },
      { "<", "<=", ">", ">=", NULL },
      { "==", "!=", NULL },
      { "&!", NULL },
      { "^", NULL },
      { "|", NULL },
      { "&&", NULL },
      { "||", NULL },
      { "=", "+=", "-=", "*=", "/=", "%=", /* "&=", "|=", "^=",*/ NULL },
      // Special keywords (lowest precedence): 15 -> 18
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
  return ((prec == 14 || prec == 4)
          ? false
          : true);
}

////////////////////
// Error checking:
////////////////////

bool
check (char **input, const char *f_name, int start_line, int *lines)
{
  /**
   * check - check for errors potentially harmful to the parser, and
   * print them out. Among other things, this function checks for
   * incompletions.
   */
  
  int i, j;
  int prec;
  int state;
  int s_size;
  int p_hold;
  int p_count, b_count, c_count;

  char *custom_fmt;
  char *next_token;
  char *prev_token;
  bool *com_stack, *def_stack, *semi_stack;

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
  p_count = b_count = c_count = 0;

  for (i = 0; input[i] != NULL; i++)
    {
      next_token = input[i + 1];

      if (def_stack[s_size - 1])
        {
          if (op_get_prec (input[i]) == -1)
            {
              if (isnum (input[i]))
                {
                  custom_fmt = "cannot define, %s is a number";
                  goto custom_error;
                }
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
          p_count++;
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
          if (--p_count < 0)
            {
              custom_fmt = "mismatched %s";
              goto custom_error;
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
          b_count++;
          state = OP;
        }
      else if (!strcmp (input[i], "]"))
        {
          if (state != VAR)
            goto error;
          if (--b_count < 0)
            {
              custom_fmt = "mismatched %s";
              goto custom_error;
            }
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
          c_count++;
        }
      else if (!strcmp (input[i], "}"))
        {
          if (--c_count < 0)
            {
              custom_fmt = "mismatched %s";
              goto custom_error;
            }
          if (prev_token == NULL || (strcmp (prev_token, ";") && strcmp (prev_token, "}")))
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
            {
              if (!strcmp (input[i], "&"))
                {
                  input[i] = "&!";
                  state = OP;
                }
              else
                goto error;
            }
          else if (strcmp (next_token, "("))
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
          p_hold = p_count + 1;
          p_count = 1;
          for (j = i + 2; p_count > 0; j++)
            {
              if (input[j] == NULL)
                {
                  custom_fmt = "expected '( exp )' after %s";
                  goto custom_error;
                }
              if (!strcmp (input[j], "("))
                p_count++;
              else if (!strcmp (input[j], ")"))
                p_count--;
            }
          p_count = p_hold;
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
          p_hold = p_count + 1;
          p_count = 1;
          for (j = i + 2; p_count > 0; j++)
            {
              if (input[j] == NULL)
                {
                  custom_fmt = "expected '( exp )' after %s";
                  goto custom_error;
                }
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
              if (!strcmp (input[i], "-")
                  && (prev_token == NULL || strcmp (prev_token, "--")))
                input[i] = "--";
              else if (!strcmp (input[i], "~")
                       && (prev_token == NULL || strcmp (prev_token, "~!")))
                input[i] = "~!";
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

  if (p_count)
    {
      custom_fmt = "unmatched '('";
      goto custom_error;
    }
  else if (b_count)
    {
      custom_fmt = "unmatched '['";
      goto custom_error;
    }
  else if (c_count)
    {
      custom_fmt = "unmatched '{'";
      goto custom_error;
    }

  FACT_free (com_stack);
  FACT_free (def_stack);
  FACT_free (semi_stack);
  return false;

 error:
  custom_fmt = "unexpected %s";
 custom_error:
  // Get the line number of the error.
  for (j = i; j >= 0; j--)
    start_line += lines[j];

  // Print the error.
  fprintf (stderr, "E> Parsing error in (%s) on line %d: ",  f_name, start_line);
  fprintf (stderr, custom_fmt, input[i]);
  fprintf (stderr, "\n");

  // Free the stacks.
  FACT_free (com_stack);
  FACT_free (def_stack);
  FACT_free (semi_stack);
  return true;
}

char **
parse (char **input, const char *f_name, int start_line, int **lines)
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
  *lines = get_newlines (input);

  // Remove newlines
  for (i = 0; input[i] != NULL; i++)
    {
      if (input[i][0] == '"')
        i += 2;
      else if (input[i][0] == '\n')
        {
          for (j = i; input[j] != NULL; j++)
            input[j] = input[j + 1];
          i--;
        }
    }

  if (check (input, f_name, start_line, *lines))
    return NULL;

  output = FACT_malloc (sizeof (char *) * (i + 1));
  op_stack = FACT_malloc (sizeof (char *) * (i + 1));

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

/////////////////////
// Misc. functions: 
/////////////////////

int *
get_newlines (char **exp)
{
  int i, j, k, l;
  bool ignore;
  int *ret_val;

  ret_val = NULL;
  ignore = false;

  for (i = k = 0; exp[i + k] != NULL; i++)
    {
      ret_val = FACT_realloc (ret_val, sizeof (int) * (i + 1));
      if (!ignore && i > 0 && exp[i + k - 1][0] == '"')
        {
          for (j = l = 0; exp[i + k][l] != '\0'; l++)
            {
              if (exp[i + k][l] == '\n')
                j++;
            }
          ignore = true;
        }
      else
        {
          if (exp[i + k][0] != '"')
            ignore = false;
          for (j = 0; exp[i + k][j] == '\n'; j++);
          if (j != 0)
            k++;
        }
      ret_val[i] = j;
    }

  ret_val = FACT_realloc (ret_val, sizeof (int) * (i + 1));
  ret_val[i] = -1;
  
  return ret_val;
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
