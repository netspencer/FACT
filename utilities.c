#include "utilities.h"

char *
combine_strs (char *str1, char *str2)
{
  int new_length;
  int scroll1;
  int scroll2;
  char *new_string;

  new_length = strlen (str1) + strlen (str2);
  new_string = (char *) better_malloc (sizeof (char) * new_length);

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
array_to_string (var *convertable)
{
  var *scroller;
  char *return_value;
  char *setter;

  return_value = (char *) better_malloc (sizeof (char) * convertable->array_size);
  setter = return_value;

  if (convertable->array_size > 1)
    {
      for (scroller = convertable->array_up; scroller != NULL; scroller = scroller->next, setter++)
	setter[0] = mpc_get_si (scroller->data);
    }
  else
    setter[0] = mpc_get_si (convertable->data);

  return return_value;
}

var *
string_to_array (char *convertable, char *name)
{
  int length;
  int pos;
  var *root;
  var *scroller;

  length = strlen (convertable) + 1;

  root = alloc_var ();

  for (scroller = root, pos = 1; pos < length; pos++)
    {
      scroller->name = name;
      mpc_set_si (&(scroller->data), (int) (convertable [pos - 1]));
      scroller->next = alloc_var ();
      scroller = scroller->next;
    }

  scroller->name = name;

  return root;
}

var *
string_array_to_var (char **strings, char *var_name, int array_size)
{
  int pos;
  var *root;
  var *scroller;
  
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
      scroller->array_size = strlen (strings[pos]) + 1;
    }

  return root;
}
