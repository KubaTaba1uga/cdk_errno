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

struct cme_Error *cme_ringbuf = NULL;
uint32_t cme_ringbuf_i = 0;

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

void cme_error_destroy(cme_error_t err) {}

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
