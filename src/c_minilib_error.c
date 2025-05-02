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

#define CME_RING_SIZE 128

static struct cme_Error *cme_ringbuf = NULL;
static uint32_t cme_ringbuf_i = 0;
static inline uint32_t next_idx(void) {
  return cme_ringbuf_i++ & (CME_RING_SIZE - 1);
}

int cme_init(void) {
  if (cme_ringbuf) {
    return 0;
  }

  cme_ringbuf = malloc(CME_RING_SIZE * sizeof(struct cme_Error));
  if (!cme_ringbuf) {
    return ENOMEM;
  }

  cme_ringbuf_i = 0;

  return 0;
}

void cme_destroy(void) {
  if (!cme_ringbuf) {
    return;
  }

  free(cme_ringbuf);
  cme_ringbuf = NULL;
  cme_ringbuf_i = 0;
}

cme_error_t cme_error_create(int code, char *source_file, char *source_func,
                             int source_line, const char *msg) {

  cme_error_t err = &cme_ringbuf[next_idx()];
  err->code = code;
  err->msg = msg;
  err->frames_length = 1;
  err->frames[0] = (struct cme_Frame){
      .file = source_file, .func = source_func, .line = source_line};
  return err;
}
cme_error_t cme_error_create_fmt(int code, char *source_file, char *source_func,
                                 int source_line, const char *fmt, ...) {
  cme_error_t err = &cme_ringbuf[next_idx()];

  /* err->code = code; */
  /* err->trace_l.source_file = source_file; */
  /* err->trace_l.source_func = source_func; */
  /* err->trace_l.source_line = source_line; */
  /* err->trace_l.next = NULL; */

  // To do create str buffer for messages
  /* if (fmt) { */
  /*   va_list args; */
  /*   va_start(args, fmt); */
  /*   vsnprintf(err->msg, CME_STR_MAX, fmt, args); */
  /*   va_end(args); */
  /* } else { */
  /*   snprintf(err->msg, CME_STR_MAX, "No message"); */
  /* } */

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

  for (size_t i = 0; i < err->frames_length; i++) {
    struct cme_Frame *sym = &err->frames[i];
    written = cme_sprintf(buffer + offset, n - offset, "%u:%s:%s:%u\n", i,
                          sym->func ? sym->func : "??",
                          sym->file ? sym->file : "??", sym->line);
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
#ifdef CME_ENABLE_BACKTRACE
  if (err->frames_length < CME_STACK_MAX) {
    err->frames[err->frames_length++] = (struct cme_Frame){file, func, line};
  }
#else
  (void)file;
  (void)func;
  (void)line;
#endif
  return err;
}
