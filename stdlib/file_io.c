#include "file_io.h"

FACT_t
run_file_soft (func_t *scope)
{
  char   *filename;
  func_t *in;

  in = get_func (scope, "in");
  filename = array_to_string (get_var (scope, "filename"));
  return run_file (in, filename, false);
}

FACT_t
run_file_loud (func_t *scope)
{
  char   *filename;
  func_t *in;

  in = get_func (scope, "in");
  filename = array_to_string (get_var (scope, "filename"));
  return run_file (in, filename, true);
}

FACT_t
open_file (func_t *scope)
{
  FACT_t return_value;
  char *filename;
  char *mode;
  FILE *return_object;
    
  filename = array_to_string (get_var (scope, "filename"));
  mode = array_to_string (get_var (scope, "mode"));

  return_object = fopen (filename, mode);

  if (return_object == NULL)
    FACT_ret_error (scope, combine_strs ("could not open file ", filename));

  return_value.type = FUNCTION_TYPE;
  return_value.f_point = alloc_func ();
  return_value.f_point->name = filename;
  return_value.f_point->usr_data = (void *) return_object;

  return return_value;
}

FACT_t
close_file (func_t *scope)
{
  func_t *file_object;

  file_object = get_func (scope, "file_object");

  if (file_object->usr_data == NULL)
    FACT_ret_error (scope, "file object is invalid");

  return FACT_get_si (fclose ((FILE *) file_object->usr_data));
}

FACT_t
get_char_file (func_t *scope)
{
  func_t *file_object;
  
  file_object = get_func (scope, "file_object");

  if (file_object->usr_data == NULL)
    FACT_ret_error (scope, "file object is invalid");

  return FACT_get_si (fgetc ((FILE *) file_object->usr_data));
}
