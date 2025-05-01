/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
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

#define CREATE_GENERIC_ERROR(status_code, err_msg)                             \
  generic_error.code = (status_code);                                          \
  generic_error.msg = (err_msg);                                               \
  generic_error.source_func = (char *)__func__;                                \
  generic_error.source_line = __LINE__

cme_error_t cme_error_create(int code, char *source_file, char *source_func,
                             int source_line, char *fmt, ...) {
  cme_error_t err = malloc(sizeof(struct cme_Error));
  if (!err) {
    CREATE_GENERIC_ERROR(ENOMEM,
                         "Unable to allocate memory for `struct cme_Error`");
    return &generic_error;
  }

  //// Fill error metadata
  err->code = code;
  err->source_line = source_line;
  err->source_file = source_file ? strdup(source_file) : NULL;
  err->source_func = source_func ? strdup(source_func) : NULL;
  err->msg = NULL;

  //// Fill error backtrace
#ifdef CME_ENABLE_BACKTRACE
  // We need to skip two first frames to do not show user this func in trace
  void *frames_buffer[CME_MAX_BACKTRACE_FRAMES + 2];
  int total_frames = backtrace(frames_buffer, CME_MAX_BACKTRACE_FRAMES + 2);
  if (total_frames > 2) {
    err->stack_size = total_frames - 2;
    err->stack_symbols = backtrace_symbols(frames_buffer + 2, err->stack_size);
  } else {
    err->stack_size = 0;
    err->stack_symbols = NULL;
  }
#else
  err->stack_size = 0;
  err->stack_symbols = NULL;
#endif

  //// Fill error message
  if (fmt) {
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    if (len < 0) {
      cme_error_destroy(err);
      CREATE_GENERIC_ERROR(EINVAL, "Invalid `struct cme_Error` variadic args");
      return &generic_error;
    }

    err->msg = malloc(len + 1);
    if (!err->msg) {
      cme_error_destroy(err);
      CREATE_GENERIC_ERROR(
          ENOMEM, "Unable to allocate memory for `struct cme_Error` message");
      return &generic_error;
    }

    va_start(args, fmt);
    vsnprintf(err->msg, len + 1, fmt, args);
    va_end(args);
  }

  return err;
}

void cme_error_destroy(cme_error_t err) {
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

int cme_error_dump(cme_error_t err, char *path) {
  const int buffer_max = 1024;
  char buffer[buffer_max];
  int offset = 0;
  int written_bytes;

  memset(buffer, 0, buffer_max);

  /* Write the initial error dump */
  written_bytes =
      snprintf(buffer, buffer_max,
               "====== ERROR DUMP ======\n"
               "Error code: %d \n"
               "Error code stringified: %s \n"
               "Error message: %s \n"
               "Src file: %s \n"
               "Src line: %d \n"
               "Src func: %s \n",
               err->code, strerror(err->code), err->msg ? err->msg : "NULL",
               err->source_file ? err->source_file : "NULL", err->source_line,
               err->source_func ? err->source_func : "NULL");

  if (written_bytes < 0 || written_bytes >= buffer_max) {
    return ENOBUFS;
  }
  offset = written_bytes;

#ifdef CME_ENABLE_BACKTRACE
  if (err->stack_symbols && err->stack_size) {
    /* Append a separator (24 '-' characters) */
    if (offset + 25 >= buffer_max) { // +1 for a newline
      return ENOBUFS;
    }
    memset(buffer + offset, '-', 24);
    offset += 24;
    buffer[offset++] = '\n';

    /* Append backtrace symbols */
    for (int i = 0; i < err->stack_size; i++) {
      written_bytes = snprintf(buffer + offset, buffer_max - offset, "%s\n",
                               err->stack_symbols[i]);
      if (written_bytes < 0 || written_bytes >= buffer_max - offset) {
        return ENOBUFS;
      }
      offset += written_bytes;
    }
  }
#endif

  /* Write the full dump to file */
  FILE *file = fopen(path, "w");
  if (!file) {
    return errno;
  }

  written_bytes = fwrite(buffer, sizeof(char), offset, file);
  fclose(file);

  if (written_bytes != offset) {
    return ENOBUFS;
  }

  return 0;
}
