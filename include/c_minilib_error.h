/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */
#ifndef C_MINILIB_ERROR_H
#define C_MINILIB_ERROR_H

#include <inttypes.h>
#include <stdint.h>

// We are staticly restricting fields to create only one memory allocation for
//  whole error. Mallocs are pretty expensive and errors can be created and
//  destroyed very frequently.
#define CME_STACK_MAX 16
#define CME_STR_MAX 255

// Because we want to hit cpu hot cache when operating on error
// error should be less than 32kib, as this is most common
// 64bit system L1 cache these days.
struct __attribute__((aligned(8))) cme_Error {
  uint32_t code;                 // 32/8 = 4 bytes
  char msg[CME_STR_MAX];         // 1*256 = 256 bytes
  char source_file[CME_STR_MAX]; // 1*256 = 256 bytes
  char source_func[CME_STR_MAX]; // 1*256 = 256 bytes
  uint32_t source_line;          // 32/8 = 4 bytes
#ifdef CME_ENABLE_BACKTRACE
  uint32_t stack_length;              // 32/8 = 4 bytes
  void *stack_symbols[CME_STACK_MAX]; // (64/8) * 16 = 128 bytes
#else
  uint32_t stack_length; // 32/8 = 4 bytes
  void **stack_symbols;  // 64/8 = 8 bytes
#endif
};
// If CME_ENABLED_BACKTRACE defined we have 4+256*3+8+4+4+128 = 916 bytes.
// If not we have 4+256*3+8+8+4+4+8 = 804 bytes.
// Because 916%8!=0 nor 804%8!=0 we need to align our struct to 8 bytes
//  via compiler `aligned` attribute.

typedef struct cme_Error *cme_error_t;

// Create error
cme_error_t cme_error_create(int code, char *source_file, char *source_func,
                             int source_line, char *fmt, ...);

// Destroy error
void cme_error_destroy(cme_error_t);

#define cme_errorf(code, fmt, ...)                                             \
  cme_error_create((code), __FILE__, (char *)__func__, __LINE__,               \
                   (char *)(fmt), ##__VA_ARGS__)

// Dump error to file
int cme_error_dump_to_file(cme_error_t err, char *file_path);

// Dump error to string
int cme_error_dump_to_str(cme_error_t err, uint32_t n, char *buffer);

static inline cme_error_t cme_return(cme_error_t err) {
#ifndef CME_ENABLE_BACKTRACE
  return err;
#else
  if (!err) {
    return err;
  }

  if (err->stack_length < CME_STACK_MAX) {
    err->stack_symbols[err->stack_length++] =
        __builtin_extract_return_addr(__builtin_return_address(0));
  }

  return err;
#endif
}

#endif // C_MINILIB_CONFIG_ERROR_H
