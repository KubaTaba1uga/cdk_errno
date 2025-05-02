#ifndef C_MINILIB_ERROR_H
#define C_MINILIB_ERROR_H

#include <stdint.h>
#include <stdio.h>

#define CME_STR_MAX 255
#define CME_STACK_MAX 1

struct cme_StackSymbol {
  const char *source_file;
  const char *source_func;
  uint32_t source_line;
};

struct cme_Error {
  uint32_t code;
  char msg[CME_STR_MAX];
  uint32_t stack_length;
  struct cme_StackSymbol stack_symbols[CME_STACK_MAX];
};

typedef struct cme_Error *cme_error_t;

int cme_error_dump_to_str(cme_error_t err, uint32_t n, char *buffer);

#endif // C_MINILIB_ERROR_H
