#include "c_minilib_error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RINGBUF_SIZE 8

static struct cme_Error ringbuf[RINGBUF_SIZE];
static unsigned int ring_index = 0;

static cme_error_t ringbuf_alloc(void) {
  struct cme_Error *err = &ringbuf[ring_index];
  memset(err, 0, sizeof(*err));
  ring_index = (ring_index + 1) % RINGBUF_SIZE;
  return err;
}

cme_error_t cme_errorf_static(int code, const char *fmt, ...) {
  cme_error_t err = ringbuf_alloc();
  if (!err)
    return NULL;

  err->code = code;
  err->stack_length = 1;
  err->stack_symbols[0].source_file = __FILE__;
  err->stack_symbols[0].source_func = __func__;
  err->stack_symbols[0].source_line = __LINE__;

  va_list args;
  va_start(args, fmt);
  vsnprintf(err->msg, CME_STR_MAX, fmt, args);
  va_end(args);

  return err;
}

int cme_error_dump_to_str(cme_error_t err, uint32_t n, char *buffer) {
  if (!err || !buffer || n == 0)
    return -1;

  return snprintf(buffer, n, "ERROR %d: %s\n at %s:%s:%u\n", err->code,
                  err->msg, err->stack_symbols[0].source_func,
                  err->stack_symbols[0].source_file,
                  err->stack_symbols[0].source_line);
}
