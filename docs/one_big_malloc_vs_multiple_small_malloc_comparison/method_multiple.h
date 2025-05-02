/* dynamic.h */
#ifndef C_MINILIB_DYNAMIC_ERROR_H
#define C_MINILIB_DYNAMIC_ERROR_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

/* how many frames to capture at most */
#define CME_STACK_MAX 6

// Because we want to hit cpu hot cache when operating on error
// error should be less than 32kib, as this is most common
// 64bit system L1 cache these days.
struct __attribute__((aligned(8))) cme_DynamicError {
  int32_t code;        // 32/8 = 4 bytes
  char *msg;           // 64/8 = 8 bytes
  char *source_file;   // 64/8 = 8 bytes
  char *source_func;   // 64/8 = 8 bytes
  int32_t source_line; // 32/8 = 4 bytes
#ifdef CME_ENABLE_BACKTRACE
  int32_t stack_length;          // 32/8 = 4 bytes
  struct cme_StackSymbol *stack; // 64/8 = 8 bytes
#else
  // This is not filled, we just want to keep API happy.
  uint32_t stack_length; // 32/8 = 4 bytes
  struct cme_StackSymbol *stack;
#endif
};
// We have 4+8+8+8+4+4+8 = 44 bytes.
// Because 44%8!=0 we need to align our struct to 8 bytes
//  via compiler `aligned` attribute so it becomes 48 bytes.
// But size of the struct is not fully reflecting real size of error
// because each string also takes memory.

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

/* ── internal frame-push helper ───────────────────────────────────── */
static inline cme_dynamic_error_t
cme_dynamic_push_frame(cme_dynamic_error_t err, const char *file,
                       const char *func, int line) {
#ifndef CME_ENABLE_BACKTRACE
  (void)file;
  (void)func;
  (void)line;
  return err;
#else
  if (!err)
    return err;
  if (err->stack_length >= CME_STACK_MAX)
    return err;

  struct cme_StackSymbol *f = &err->stack[err->stack_length++];
  f->source_file = file;
  f->source_func = func;
  f->source_line = line;
  return err;
#endif
}

/* ── user-facing macro ───────────────────────────────────────────── */
#define cme_return(ERR)                                                        \
  cme_dynamic_push_frame((ERR), __FILE__, __func__, __LINE__)

#endif // C_MINILIB_DYNAMIC_ERROR_H
