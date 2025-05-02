/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */
#ifndef C_MINILIB_ERROR_H
#define C_MINILIB_ERROR_H

/* We have these settings available: */
/* - CME_ENABLE_BACKTRACE, enable backtrace gathering in code, small performance
 *   hit (like 2x) for gathering return path. */

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>

#define CME_STR_MAX 255

// Resolving symbols is quite cumbersome and require a lot of compilation flags
// like -fno-omit-pointer, -fno-pie etc. Resolving symbols on compilation
//  step simplify process much.

#define CME_STACK_MAX 8 /* 5 is enough, 8 keeps it 64-byte aligned */

struct cme_Frame {
  const char *file;
  const char *func;
  uint32_t line;
};

struct __attribute__((aligned(64))) cme_Error {
  uint8_t code;
  const char *msg;
  uint8_t frames_length;
  struct cme_Frame frames[CME_STACK_MAX];
};

typedef struct cme_Error *cme_error_t;

int cme_init(void);

void cme_destroy(void);

#define CME_RING_SIZE 64

// Create error
// expose the one true ring
extern struct cme_Error cme_ringbuf[CME_RING_SIZE];
extern uint32_t cme_ringbuf_i;

#define CME_RING_MASK (CME_RING_SIZE - 1)

// always-inline indexer
static inline __attribute__((always_inline)) uint32_t next_idx(void) {
  uint32_t i = cme_ringbuf_i;
  cme_ringbuf_i = (i + 1) & CME_RING_MASK;
  return i;
}

// always-inline create
static inline __attribute__((always_inline)) cme_error_t
cme_error_create(int code, const char *file, const char *func, int line,
                 const char *msg) {
  cme_error_t e = &cme_ringbuf[next_idx()];
  e->code = (uint8_t)code;
  e->msg = msg;
  e->frames_length = 1;
  e->frames[0] = (struct cme_Frame){file, func, (uint32_t)line};
  return e;
}

// always-inline push
static inline __attribute__((always_inline)) cme_error_t
cme_error_push_symbol(cme_error_t err, const char *file, const char *func,
                      int line) {
#ifdef CME_ENABLE_BACKTRACE
  if (err->frames_length < CME_STACK_MAX)
    err->frames[err->frames_length++] =
        (struct cme_Frame){file, func, (uint32_t)line};
#else
  (void)file;
  (void)func;
  (void)line;
#endif
  return err;
}
/* cme_error_t cme_error_create(int code, char *source_file, char *source_func,
 */
/*                              int source_line, const char *msg); */
cme_error_t cme_error_create_fmt(int code, char *source_file, char *source_func,
                                 int source_line, const char *fmt, ...);

// Destroy error
void cme_error_destroy(cme_error_t err);

// Handy macros
#define cme_errorf(code, fmt, ...)                                             \
  cme_error_create_fmt((code), __FILE__, (char *)__func__, __LINE__,           \
                       (char *)(fmt), ##__VA_ARGS__)

// Handy macro
#define cme_error(code, msg)                                                   \
  cme_error_create((code), __FILE__, (char *)__func__, __LINE__,               \
                   (const char *)(msg))

// Dump error to file
int cme_error_dump_to_file(cme_error_t err, char *file_path);

// Dump error to string
int cme_error_dump_to_str(cme_error_t err, uint32_t n, char *buffer);

// Add symbol to error stack
/* cme_error_t cme_error_push_symbol(cme_error_t err, const char *file, */
/*                                   const char *func, int line); */
// Handy macro
#define cme_return(ERR)                                                        \
  cme_error_push_symbol((ERR), __FILE__, __func__, __LINE__)

#endif // C_MINILIB_CONFIG_ERROR_H
