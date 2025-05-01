/* dynamic.h */
#ifndef C_MINILIB_DYNAMIC_ERROR_H
#define C_MINILIB_DYNAMIC_ERROR_H

#include <stdint.h>
#include <stdio.h>

/* how many frames to capture at most */
#define CME_STACK_MAX 16

// Because we want to hit cpu hot cache when operating on error
// error should be less than 32kib, as this is most common
// 64bit system L1 cache these days.
struct __attribute__((aligned(8))) cme_DynamicError {
  int32_t code;            // 32/8 = 4 bytes
  char *msg;               // 64/8 = 8 bytes
  const char *source_file; // 64/8 = 8 bytes
  const char *source_func; // 64/8 = 8 bytes
  int32_t source_line;     // 32/8 = 4 bytes
  int32_t stack_length;    // 32/8 = 4 bytes
  void **stack_addrs;      // 64/8 = 8 bytes
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

#endif // C_MINILIB_DYNAMIC_ERROR_H
