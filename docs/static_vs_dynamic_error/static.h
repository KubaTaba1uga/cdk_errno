#ifndef C_MINILIB_STATIC_ERROR_H
#define C_MINILIB_STATIC_ERROR_H

#include <stdio.h>

#define CME_STACK_MAX 16
#define CME_STR_MAX 255

struct cme_StaticError {
  int code;
  char msg[CME_STR_MAX];
  // These needs to be string literals
  const char *source_file;
  const char *source_func;
  int source_line;
#ifdef CME_ENABLE_BACKTRACE
  int stack_length;
  // We store ptrs to funcs only
  void *stack_symbols[CME_STACK_MAX];
#else
  // This is not filled, we just want to keep API happy.
  int stack_length;
  void **stack_symbols;
#endif
};

typedef struct cme_StaticError *cme_static_error_t;

// Allocate and create error
cme_static_error_t cme_static_error_create(int code, const char *file,
                                           const char *func, int line,
                                           const char *fmt, ...);

// Destroy and free
void cme_static_error_destroy(cme_static_error_t err);

// Dump error to file
int cme_static_error_dump(cme_static_error_t err, const char *path);

#define cme_static_errorf(code, fmt, ...)                                      \
  cme_static_error_create((code), __FILE__, __func__, __LINE__, (fmt),         \
                          ##__VA_ARGS__)

#endif // C_MINILIB_STATIC_ERROR_H
