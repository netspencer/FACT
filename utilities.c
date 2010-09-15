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
