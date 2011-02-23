#include "FACT.h"

void
set_array (bool *rop, int op)
{
  while (op)
    rop[--op] = false;
}

int
count_until_NULL (char **words)
{
  int pos;
  for (pos = 0; words[pos] != NULL; pos++);
  return pos;
}

bool
compare_var_arrays (var_t *op1, var_t *op2, bool first)
{
  /* compare_var_arrays: returns true on == and false on !=. */ 

  if (op1 == NULL && op2 == NULL)
    return true;
  if (op1 == NULL || op2 == NULL)
    return false;
  
  if (!mpc_cmp (op1->data, op2->data))
    return compare_var_arrays (op1->array_up, op2->array_up, false)
      && ((first) // If we're not in the first position, search the 'next' element.
          ? true
          : compare_var_arrays (op1->next, op2->next, false));
  return false;
}

char *
combine_strs (char *str1, char *str2)
{
  int    new_length;
  int    scroll1;
  int    scroll2;
  char * new_string;

  new_length = strlen (str1) + strlen (str2);
  new_string = (char *) better_malloc (sizeof (char) * (new_length + 1));

  for (scroll1 = 0; str1[scroll1] != '\0'; scroll1++)
    new_string[scroll1] = str1[scroll1];

  for (scroll2 = 0; str2[scroll2] != '\0'; scroll1++, scroll2++)
    new_string[scroll1] = str2[scroll2];

  new_string[scroll1 + 1] = '\0';

  return new_string;
}

char *
rm_cslashes (char *op)
{
  /* rm_cslashes - remove slashes and escape characters in C, replacing
   * them with their equivalent ascii code.
   */
  int    index;
  int    jndex;
  char * new;

  new = better_malloc (sizeof (char) * strlen (op));

  for (index = jndex = 0; op[index] != '\0'; jndex++, index++)
    {
      if (op[index] == '\\')
        {
          switch (op[index + 1])
            {
            case 'b':
              new[jndex] = '\b';
              break;

            case 'f':
              new[jndex] = '\f';
              break;

            case 'n':
              new[jndex] = '\n';
              break;

            case 'r':
              new[jndex] = '\r';
              break;

            case 't':
              new[jndex] = '\t';
              break;

            case '\\':
              new[jndex] = '\\';
              break;

            case '"':
              new[jndex] = '"';
              break;

            case '\0':
              new[jndex] = '\\';
              index--;
              break;
              
            default:
              new[jndex] = '\\';
              new[jndex + 1] = op[index + 1];
            }
          index++;
        }
      else
        new[jndex] = op[index];
    }

  new = better_realloc (new, sizeof (char) * (jndex + 1));
  new[jndex] = '\0';

  return new;
}

char **
copy (char **words)
{
  int     pos;
  int     count;
  char ** temp;

  if (words == NULL)
    return NULL;

  for (pos = 0; *(words + pos) != NULL; pos++);

  temp = (char **) better_malloc (sizeof (char *) * (pos + 1));

  for (count = 0; count < pos; count++)
    temp[count] = words[count];

  temp[count] =  NULL;

  return temp;
}

char *
array_to_string (var_t *convertable)
{
  char  * return_value;
  char  * setter;
  var_t * scroller;
  
  return_value = better_malloc (sizeof (char) * (mpz_get_ui (convertable->array_size) + 1));
  setter       = return_value;

  if (mpz_cmp_ui (convertable->array_size, 1) > 0)
    {
      for (scroller = convertable->array_up; scroller != NULL; scroller = scroller->next, setter++)
	setter[0] = mpc_get_si (scroller->data);
    }
  else
    setter[0] = mpc_get_si (convertable->data);

  return return_value;
}

var_t *
string_to_array (char *convertable, char *name)
{
  int     index;
  var_t * root;
  var_t * scroller;

  root     = alloc_var ();
  scroller = root;
  
  for (index = 0; convertable[index] != '\0'; index++)
    {
      mpc_set_si (&(scroller->data), (int) (convertable [index]));
      scroller->name = name;
      
      if (convertable[index + 1] != '\0')
        {
          scroller->next = alloc_var ();
          scroller       = scroller->next;
        }
    }

  return root;
}

var_t *
string_array_to_var (char **strings, char *var_name, int array_size)
{
  int     pos;
  var_t * root;
  var_t * scroller;
  
  root       = alloc_var ();
  root->name = var_name;

  for (scroller = root, pos = 0; pos < array_size; pos++)
    {
      if (pos != 0)
	{
	  scroller->next = alloc_var ();
	  scroller       = scroller->next;
	}

      scroller->name       = var_name;
      scroller->array_up   = string_to_array (strings[pos], var_name);
      mpz_set_si (scroller->array_size, strlen (strings[pos]) + 1);
    }

  return root;
}

FACT_t
FACT_get_ui (unsigned int op)
{
  FACT_t ret;

  ret.type          = VAR_TYPE;
  ret.return_signal = false;
  ret.break_signal  = false;
  ret.v_point       = alloc_var ();
  mpc_set_ui (&(ret.v_point->data), op);

  return ret;
}

FACT_t
FACT_get_si (signed int op)
{
  FACT_t ret;

  ret.type          = VAR_TYPE;
  ret.return_signal = false;
  ret.break_signal  = false;
  ret.v_point       = alloc_var ();
  mpc_set_si (&(ret.v_point->data), op);

  return ret;
}
  
int
tokcmp_safe (const char *str1, const char *str2, int line, char *file)
{
  if (str1 == NULL)
    {
      fprintf (stderr, "Call to tokcmp_safe with NULL as first argument at %s:%d. Aborting...\n", file, line);
      abort ();
    }
  if (str2 == NULL)
    {
      fprintf (stderr, "Call to tokcmp_safe with NULL as second argument at %s:%d. Aborting...\n", file, line);
      abort ();
    }

  while (*str1 == '\n')
    str1++;
  while (*str2 == '\n')
    str2++;
  
  return strcmp (str1, str2);
}

unsigned int
strcount (const char op1, const char *op2)
{
  int index;
  
  for (index = 0; *op2 != '\0'; op2++)
    {
      if (op1 == *op2)
	index++;
    }
  return 0;
}
