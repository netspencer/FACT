#include "file_io.h"

FACT_t
run_file_soft (func_t *scope)
{
  /**
   * Name: run
   * Arguments: def filename, defunc in
   * Returns: the result of run_file
   * On error: None.
   */
  char   * filename;
  func_t * in;

  in = get_func (scope, "in");

  filename = array_to_string (get_var (scope, "filename"));
  return run_file (in, filename, false);
}

FACT_t
run_file_loud (func_t *scope)
{
  /**
   * Name: run_loud
   * Arguments: def filename, defunc in
   * Returns: The result of run_file
   * On error: None.
   */
  char   * filename;
  func_t * in;

  in = get_func (scope, "in");

  filename = array_to_string (get_var (scope, "filename"));
  return run_file (in, filename, true);
}

FACT_t
open_file (func_t *scope)
{
  /**
   * Name: fopen
   * Arguments: def filename, def mode
   * Returns: function with file structure in usr_data
   * On error: throws an error
   */
  char   * filename;
  char   * mode;
  FILE   * return_object;
  FACT_t   return_value;

  filename = array_to_string (get_var (scope, "filename"));
  mode     = array_to_string (get_var (scope, "mode"    ));

  return_object = fopen (filename, mode);
  if (return_object == NULL)
    return errorman_throw_catchable (scope, combine_strs ("could not open file ", filename));
  return_value.type              = FUNCTION_TYPE;
  return_value.f_point           = alloc_func ();
  return_value.f_point->name     = filename;
  return_value.f_point->usr_data = (void *) return_object;

  return return_value;
}

FACT_t
close_file (func_t *scope)
{
  /**
   * Name: fclose
   * Arguments: defunc file_object
   * Returns: 0
   * On error: throws an exception
   */  
  func_t * file_object;

  file_object = get_func (scope, "file_object");

  if (file_object->usr_data == NULL)
    return errorman_throw_catchable (scope, "file object is invalid");

  return FACT_get_si (fclose ((FILE *) file_object->usr_data));
}

FACT_t
get_char_file (func_t *scope)
{
  /**
   * Name: fgetc
   * Arguments: defunc file_object
   * Returns: The next available character in file_object.
   * On error: throws an exception.
   */
  func_t * file_object;
  
  file_object = get_func (scope, "file_object");

  if (file_object->usr_data == NULL)
    return errorman_throw_catchable (scope, "file object is invalid");

  return FACT_get_si (fgetc ((FILE *) file_object->usr_data));
}
