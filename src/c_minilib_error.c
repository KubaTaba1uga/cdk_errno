/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef CME_ENABLE_BACKTRACE
#include <execinfo.h>
#define CME_MAX_BACKTRACE_FRAMES 16
#endif

#include "c_minilib_error.h"

static struct cme_Error generic_error = {
    .code = -1,
    .msg = "Generic error message",
    .source_file = __FILE__,
    .source_func = 0,
    .source_line = 0,
    .stack_size = 0,
    .stack_symbols = NULL,
};

struct cme_Error *cme_error_create(int code, char *source_file,
                                   char *source_func, int source_line,
                                   char *fmt, ...) {
  struct cme_Error *err = malloc(sizeof(struct cme_Error));
  if (!err) {
    generic_error.code = ENOMEM;
    generic_error.msg = "Unable to allocate memory for `struct cme_Error`";
    generic_error.source_func = (char *)__func__;
    generic_error.source_line = __LINE__;
    return &generic_error;
  }

  err->code = code;
  err->source_line = source_line;
  err->source_file = source_file ? strdup(source_file) : NULL;
  err->source_func = source_func ? strdup(source_func) : NULL;
  err->msg = NULL;

#ifdef CME_ENABLE_BACKTRACE
  void *buffer[CME_MAX_BACKTRACE_FRAMES + 1];
  err->stack_size = backtrace(buffer, CME_MAX_BACKTRACE_FRAMES + 1) - 1;
  err->stack_symbols = backtrace_symbols(buffer + 1, err->stack_size);
#else
  err->stack_size = 0;
  err->stack_symbols = NULL;
#endif

  if (fmt) {
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (len < 0) {
      cme_error_destroy(err);
      generic_error.code = EINVAL;
      generic_error.msg = "Invalid `struct cme_Error` variadic args";
      generic_error.source_func = (char *)__func__;
      generic_error.source_line = __LINE__;
      return &generic_error;
    }

    err->msg = malloc(len + 1);
    if (!err->msg) {
      cme_error_destroy(err);
      generic_error.code = ENOMEM;
      generic_error.msg =
          "Unable to allocate memory for `struct cme_Error` message";
      generic_error.source_func = (char *)__func__;
      generic_error.source_line = __LINE__;
      return &generic_error;
    }

    va_start(args, fmt);
    vsnprintf(err->msg, len + 1, fmt, args);
    va_end(args);
  }

  return err;
}

void cme_error_destroy(struct cme_Error *err) {
  if (!err || err == &generic_error)
    return;

  free(err->msg);
  free(err->source_file);
  free(err->source_func);

#ifdef CME_ENABLE_BACKTRACE
  free(err->stack_symbols);
#endif

  free(err);
}
