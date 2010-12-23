#include "utilities.h"

void
set_array (bool *rop, int op)
{
  while (op)
    rop[--op] = false;
  /*
  for (pos = 0; pos < op; pos++)
    rop[pos] = false;
  */
}

int
count_until_NULL (char **words)
{
  int pos;
  for (pos = 0; words[pos] != NULL; pos++);
  return pos;
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
  
  return_value = (char *) better_malloc (sizeof (char) * mpz_get_ui (convertable->array_size));
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
  int     length;
  int     pos;
  var_t * root;
  var_t * scroller;

  length = strlen (convertable) + 1;
  root   = alloc_var ();

  for (scroller = root, pos = 1; pos < length; pos++)
    {
      mpc_set_si (&(scroller->data), (int) (convertable [pos - 1]));
      scroller->name = name;
      scroller->next = alloc_var ();
      scroller       = scroller->next;
    }

  scroller->name = name;

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
