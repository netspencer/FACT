#include "garbage.h"

int num_of_objects;
//int num_of_data_blocks;
void **objects;
//mpz_t **data_blocks;

void init_collection ()
{
  num_of_objects = 0;
  //num_of_data_blocks = 0;
  objects = NULL;
  //data_blocks = NULL;
}


void add_object (void *to_add)
{
  objects = (void **) better_realloc (objects, sizeof (void *) * (num_of_objects + 1));
  objects[num_of_objects] = to_add;
  num_of_objects++;
}

 /*
void add_data (mpz_t *to_add)
{
  data_blocks = (mpz_t **) better_realloc (data_blocks, sizeof (mpz_t *) * (num_of_data_blocks + 1));
  data_blocks[num_of_data_blocks] = to_add;
  num_of_data_blocks++;
  }*/

void free_cache ()
{
  int count;

  for (count = 0; count < num_of_objects; count++)
    free (objects[count]);

  /*for (count = 0; count < num_of_data_blocks; count++)
    mpz_clear (*(data_blocks[count]));*/
}
/*
void init_temp_data_list (mpz_t ***data_list, int *num)
{
  *data_list = NULL;
  *num = 0;
}
*/
