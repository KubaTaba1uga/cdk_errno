#include "c_minilib_error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

cme_error_t alloc_malloc_errorf(int code, const char *fmt, ...) {
  struct cme_Error *err = calloc(1, sizeof(*err));
  if (!err)
    return NULL;
  err->code = code;
  err->stack_length = 1;
  err->stack_symbols[0].source_func = __func__;
  err->stack_symbols[0].source_file = __FILE__;
  err->stack_symbols[0].source_line = __LINE__;

  strcpy(err->msg, fmt);

  return err;
}

void alloc_malloc_free(cme_error_t err) { cme_error_destroy(err); }
