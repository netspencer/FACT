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

int
compare_var_arrays (var_t *op1, var_t *op2, bool first)
{
  int res;
  
  if (op1 == NULL && op2 == NULL)
    return 0;
  if (op1 == NULL || op2 == NULL)
    return (op1 == NULL) ? -1 : 1;

  res = mpc_cmp (op1->data, op2->data);
  if (res == 0)
    {
      res = compare_var_arrays (op1->array_up, op2->array_up, false);
      if (res == 0 && !first)
        return compare_var_arrays (op1->next, op2->next, false);
    }
  return res;
}

char *
combine_strs (char *str1, char *str2)
{
  int new_length;
  int scroll1;
  int scroll2;
  char *new_string;

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
  int i, j;
  char *new;

  new = better_malloc (sizeof (char) * strlen (op));

  for (i = j = 0; op[i] != '\0'; j++, i++)
    {
      if (op[i] == '\\')
        {
          switch (op[i + 1])
            {
            case 'b':
              new[j] = '\b';
              break;

            case 'f':
              new[j] = '\f';
              break;

            case 'n':
              new[j] = '\n';
              break;

            case 'r':
              new[j] = '\r';
              break;

            case 't':
              new[j] = '\t';
              break;

            case '\\':
              new[j] = '\\';
              break;

            case '"':
              new[j] = '"';
              break;

            case '\0':
              new[j] = '\\';
              i--;
              break;
              
            default:
              new[j] = '\\';
              new[j + 1] = op[i + 1];
            }
          i++;
        }
      else
        new[j] = op[i];
    }

  new = better_realloc (new, sizeof (char) * (j + 1));
  new[j] = '\0';

  return new;
}

char **
copy (char **words)
{
  int pos;
  int count;
  char **temp;

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
  char  *return_value;
  char  *setter;
  var_t *scroller;
  
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
  int i;
  var_t *root;
  var_t *scroller;

  root = alloc_var ();
  scroller = root;
  
  for (i = 0; convertable[i] != '\0'; i++)
    {
      mpc_set_si (&(scroller->data), (int) (convertable [i]));
      scroller->name = name;
      
      if (convertable[i + 1] != '\0')
        {
          scroller->next = alloc_var ();
          scroller = scroller->next;
        }
    }

  return root;
}

var_t *
string_array_to_var (char **strings, char *var_name, int array_size)
{
  int pos;
  var_t *root;
  var_t *scroller;
  
  root = alloc_var ();
  root->name = var_name;

  for (scroller = root, pos = 0; pos < array_size; pos++)
    {
      if (pos != 0)
	{
	  scroller->next = alloc_var ();
	  scroller = scroller->next;
	}

      scroller->name = var_name;
      scroller->array_up = string_to_array (strings[pos], var_name);
      mpz_set_si (scroller->array_size, strlen (strings[pos]) + 1);
    }

  return root;
}

FACT_t
FACT_get_ui (unsigned int op)
{
  FACT_t ret =
    {
      .type = VAR_TYPE,
      .return_signal = false,
      .break_signal = false,
      .v_point = alloc_var (),
    };
  mpc_set_ui (&(ret.v_point->data), op);

  return ret;
}

FACT_t
FACT_get_si (signed int op)
{
  FACT_t ret =
    {
      .type = VAR_TYPE,
      .return_signal = false,
      .break_signal = false,
      .v_point = alloc_var (),
    };
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
  int i;
  
  for (i = 0; *op2 != '\0'; op2++)
    {
      if (op1 == *op2)
	i++;
    }
  
  return i;
}
