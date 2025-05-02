/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c_minilib_error.h"
#include "common.h"

static struct cme_Error generic_error = {0};

#define CREATE_GENERIC_ERROR(status_code, err_msg)                             \
  generic_error.stack_length = 0;                                              \
  generic_error.code = (status_code);                                          \
  generic_error.source_line = __LINE__;                                        \
  snprintf(generic_error.msg, CME_STR_MAX, "%s", (err_msg));                   \
  snprintf(generic_error.source_func, CME_STR_MAX, "%s", __func__);

cme_error_t cme_error_create(int code, char *source_file, char *source_func,
                             int source_line, char *fmt, ...) {
  cme_error_t err = calloc(1, sizeof(struct cme_Error));
  if (!err) {
    CREATE_GENERIC_ERROR(ENOMEM,
                         "Unable to allocate memory for `struct cme_Error`");
    return &generic_error;
  }

  err->stack_length = 0;
  err->code = code;
  err->source_line = source_line;

  if (source_file) {
    snprintf(err->source_file, CME_STR_MAX, "%s", source_file);
  }

  if (source_func) {
    snprintf(err->source_func, CME_STR_MAX, "%s", source_func);
  }

  if (fmt) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(err->msg, CME_STR_MAX, fmt, args);
    va_end(args);
  } else {
    snprintf(err->msg, CME_STR_MAX, "No message");
  }

  return err;
}

void cme_error_destroy(cme_error_t err) {
  if (!err || err == &generic_error) {
    return;
  }

  free(err);
}

int cme_error_dump_to_str(cme_error_t err, uint32_t n, char *buffer) {
  if (!err || !buffer || n == 0) {
    return EINVAL;
  }

  size_t offset = 0;
  int written;

  /* 1) Common fields */
  written = cme_sprintf(buffer + offset, n - offset,
                        "====== ERROR DUMP ======\n"
                        "Error code: %d\n"
                        "Error message: %s\n"
                        "Src file: %s\n"
                        "Src line: %d\n"
                        "Src func: %s\n",
                        err->code, err->msg, err->source_file, err->source_line,
                        err->source_func);
  if (written < 0) {
    return ENOBUFS;
  }
  offset += (size_t)written;

#ifdef CME_ENABLE_BACKTRACE
  if (err->stack_length > 0) {
    written =
        cme_sprintf(buffer + offset, n - offset, "------------------------\n");
    if (written < 0) {
      return ENOBUFS;
    }
    offset += (size_t)written;

    for (int i = 0; i < err->stack_length; ++i) {
      written = cme_sprintf(buffer + offset, n - offset, "[%p]\n",
                            err->stack_symbols[i]);
      if (written < 0) {
        return ENOBUFS;
      }
      offset += (size_t)written;
    }
  }
#endif

  return 0;
}

int cme_error_dump_to_file(cme_error_t err, char *path) {
  FILE *file = fopen(path, "w");
  if (!file) {
    return errno;
  }

  char buffer[4096];
  cme_error_dump_to_str(err, sizeof(buffer), buffer);
  fwrite(buffer, sizeof(char), strlen(buffer), file);
  fclose(file);

  return 0;
}
