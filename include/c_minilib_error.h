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
#ifdef CME_ENABLE_BACKTRACE
#define CME_STACK_MAX 16
#else
#define CME_STACK_MAX 1
#endif

#define CME_STR_MAX 255

struct cme_StackSymbol {
  const char *source_file;
  const char *source_func;
  uint32_t source_line;
};

struct __attribute__((aligned(8))) cme_Error {
  uint32_t code;
  char msg[CME_STR_MAX];
  uint32_t stack_length;
  struct cme_StackSymbol stack_symbols[CME_STACK_MAX];
};

typedef struct cme_Error *cme_error_t;

// Create error
cme_error_t cme_error_create(int code, char *source_file, char *source_func,
                             int source_line, char *fmt, ...);

// Destroy error
void cme_error_destroy(cme_error_t);

// Handy macro
#define cme_errorf(code, fmt, ...)                                             \
  cme_error_create((code), __FILE__, (char *)__func__, __LINE__,               \
                   (char *)(fmt), ##__VA_ARGS__)

// Dump error to file
int cme_error_dump_to_file(cme_error_t err, char *file_path);

// Dump error to string
int cme_error_dump_to_str(cme_error_t err, uint32_t n, char *buffer);

// Add symbol to error stack
cme_error_t cme_error_push_symbol(cme_error_t err, const char *file,
                                  const char *func, int line);
// Handy macro
#define cme_return(ERR)                                                        \
  cme_error_push_symbol((ERR), __FILE__, __func__, __LINE__)

#endif // C_MINILIB_CONFIG_ERROR_H
