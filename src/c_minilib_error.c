/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c_minilib_error.h"
#include "common.h"

static struct cme_Settings cme_settings = {.ring_size = 32,
                                           .is_ring_growable = false};
static struct cme_Error *cme_ringbuf = NULL;
static uint32_t cme_ringbuf_i = 0;

int cme_init(void) {
  if (cme_ringbuf) {
    return 0;
  }

  cme_ringbuf = calloc(cme_settings.ring_size, sizeof(struct cme_Error));
  if (!cme_ringbuf) {
    return ENOMEM;
  }

  cme_ringbuf_i = 0;

  return 0;
}

void cme_configure(struct cme_Settings *settings) {
  if (!settings) {
    return;
  }

  cme_settings.ring_size = settings->ring_size;
  cme_settings.is_ring_growable = settings->is_ring_growable;
};

void cme_destroy(void) {
  if (!cme_ringbuf) {
    return;
  }

  free(cme_ringbuf);
  cme_ringbuf = NULL;
  cme_ringbuf_i = 0;
}

cme_error_t cme_error_create(int code, char *source_file, char *source_func,
                             int source_line, char *fmt, ...) {
  cme_error_t err = &cme_ringbuf[cme_ringbuf_i++ % cme_settings.ring_size];

  err->code = code;
  err->stack_length = 1;
  err->stack_symbols[0].source_func = source_func;
  err->stack_symbols[0].source_file = source_file;
  err->stack_symbols[0].source_line = source_line;

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
  if (!err) {
    return;
  }

  memset(err, 0, sizeof(struct cme_Error));
}

int cme_error_dump_to_str(cme_error_t err, uint32_t n, char *buffer) {
  if (!err || !buffer || n == 0) {
    return EINVAL;
  }

  size_t offset = 0;
  int written;

  written = cme_sprintf(buffer + offset, n - offset,
                        "====== ERROR DUMP ======\n"
                        "Error code: %d\n"
                        "Error message: %s\n",
                        err->code, err->msg);
  if (written < 0) {
    return ENOBUFS;
  }

  offset += (size_t)written;

  written =
      cme_sprintf(buffer + offset, n - offset, "------------------------\n");
  if (written < 0) {
    return ENOBUFS;
  }
  offset += (size_t)written;

  for (uint32_t i = 0; i < err->stack_length; ++i) {
    const struct cme_StackSymbol *sym = &err->stack_symbols[i];
    written = cme_sprintf(buffer + offset, n - offset, "%u:%s:%s:%u\n", i,
                          sym->source_func ? sym->source_func : "??",
                          sym->source_file ? sym->source_file : "??",
                          sym->source_line);
    if (written < 0) {
      return ENOBUFS;
    }
    offset += (size_t)written;
  }

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

cme_error_t cme_error_push_symbol(cme_error_t err, const char *file,
                                  const char *func, int line) {
#ifndef CME_ENABLE_BACKTRACE
  (void)file;
  (void)func;
  (void)line;
  return err;
#else
  if (!err) {
    return err;
  }
  if (err->stack_length >= CME_STACK_MAX) {
    return err;
  }

  struct cme_StackSymbol *f = &err->stack_symbols[err->stack_length++];
  f->source_file = file;
  f->source_func = func;
  f->source_line = line;
  return err;
#endif
}
