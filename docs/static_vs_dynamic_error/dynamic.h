/* dynamic.h */
#ifndef C_MINILIB_DYNAMIC_ERROR_H
#define C_MINILIB_DYNAMIC_ERROR_H

#include <stdio.h>

/* how many frames to capture at most */
#define CME_STACK_MAX 32

struct cme_DynamicError {
  int code;
  char *msg;
  const char *source_file;
  const char *source_func;
  int source_line;
  int stack_length;
  void **stack_addrs;
};

typedef struct cme_DynamicError *cme_dynamic_error_t;

// Create, destroy, dump
cme_dynamic_error_t cme_dynamic_error_create(int code, const char *file,
                                             const char *func, int line,
                                             const char *fmt, ...);
void cme_dynamic_error_destroy(cme_dynamic_error_t err);
int cme_dynamic_error_dump(cme_dynamic_error_t err, const char *path);

// shorthand
#define cme_dynamic_errorf(code, fmt, ...)                                     \
  cme_dynamic_error_create((code), __FILE__, __func__, __LINE__, (fmt),        \
                           ##__VA_ARGS__)

#endif // C_MINILIB_DYNAMIC_ERROR_H
