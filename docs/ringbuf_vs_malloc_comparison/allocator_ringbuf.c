#include "c_minilib_error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RINGBUF_SIZE 32

static struct cme_Error *ringbuf = NULL;
static size_t index = 0;

void alloc_ringbuf_init(void) {
  ringbuf = calloc(RINGBUF_SIZE, sizeof(struct cme_Error));
}

void alloc_ringbuf_cleanup(void) { free(ringbuf); }

cme_error_t alloc_ringbuf_errorf(int code, const char *fmt, ...) {
  struct cme_Error *err = &ringbuf[index++ % RINGBUF_SIZE];
  err->code = code;
  err->stack_length = 1;
  err->stack_symbols[0].source_func = __func__;
  err->stack_symbols[0].source_file = __FILE__;
  err->stack_symbols[0].source_line = __LINE__;

  strcpy(err->msg, fmt);

  return err;
}
