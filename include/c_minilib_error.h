/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */
#ifndef C_MINILIB_ERROR_H
#define C_MINILIB_ERROR_H

// We are staticly restricting fields to create only one memory allocation for
//  whole error. Mallocs are just pretty expensive and errors can be created
//  abd destroyed quite often in large codebase.
#define CME_STACK_MAX 32
#define CME_STR_MAX 255

struct cme_Error {
  int code;
  char msg;
  char *source_file;
  char *source_func;
  int source_line;
  int stack_length;
  char *stack_symbols[CME_STACK_MAX];
};

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
int cme_error_dump(cme_error_t, char *);

#endif // C_MINILIB_CONFIG_ERROR_H
