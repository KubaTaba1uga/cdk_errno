/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef C_MINILIB_ERROR_H
#define C_MINILIB_ERROR_H

#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
////
//////
/******************************************************************************
   C MiniLib Error - Lightweight Error Tracing & Reporting

   This library provides a fast, allocation-free way to track and pass error
   information across stack frames. Errors are stored in a circular ring buffer
   and include file/function/line trace with optional formatted messages.

   Features:
     - Always-inline creation macros
     - Optional formatted messages (like printf)
     - Call stack recording (if enabled via macro)
     - Dump to string or file

   Usage:
     - Call `cme_init()` once at startup.
     - Use `cme_error(...)` or `cme_errorf(...)` to create an error.
     - Use `cme_return(...)` to propagate and track.
     - Call `cme_destroy()` at shutdown to free the buffer.

   Example:
     return cme_return(cme_errorf(404, "File not found: %s", path));

 ******************************************************************************/
//////////

/******************************************************************************
 *                             Config / Limits                                *
 ******************************************************************************/
#define CME_STACK_MAX 16 ///< Max stack frames recorded per error
#define CME_RING_SIZE 64 ///< Number of concurrent errors stored
#define CME_STR_MAX 256  ///< Max formatted message length
#define CME_RING_MASK (CME_RING_SIZE - 1)

/******************************************************************************
 *                             Data Types                                     *
 ******************************************************************************/
/**
 * Backtrace frame (file/function/line tuple).
 */
struct cme_Frame {
  const char *file;
  const char *func;
  uint32_t line;
};

/**
 * Error object, aligned for cache performance.
 */
struct __attribute__((aligned(64))) cme_Error {
  uint8_t code;
  const char *msg;
  uint8_t frames_length;
  struct cme_Frame frames[CME_STACK_MAX];
  char _msg_buf[CME_STR_MAX];
};

typedef struct cme_Error *cme_error_t;

/******************************************************************************
 *                             Globals                                        *
 ******************************************************************************/
/**
 * Error ring buffer storage (internal).
 */
static struct cme_Error *cme_ringbuf = NULL;

/**
 * Internal buffer write index.
 */
static uint32_t cme_ringbuf_i = 0;

/******************************************************************************
 *                             General                                        *
 ******************************************************************************/
/**
 * Initialize the error system.
 * Allocates the ring buffer.
 */
static inline int cme_init(void) {
  if (cme_ringbuf)
    return 0;
  cme_ringbuf = malloc(CME_RING_SIZE * sizeof(struct cme_Error));
  return cme_ringbuf ? 0 : ENOMEM;
}

/**
 * Free ring buffer and reset state.
 */
static inline void cme_destroy(void) {
  if (cme_ringbuf) {
    free(cme_ringbuf);
    cme_ringbuf = NULL;
    cme_ringbuf_i = 0;
  }
}

/******************************************************************************
 *                             Core Internals                                 *
 ******************************************************************************/
/**
 * Get next index in ring buffer.
 */
static inline __attribute__((always_inline)) uint32_t cme_next_idx(void) {
  uint32_t i = cme_ringbuf_i;
  cme_ringbuf_i = (i + 1) & CME_RING_MASK;
  return i;
}

/******************************************************************************
 *                             Error Creation                                 *
 ******************************************************************************/
/**
 * Create a simple error (no formatting).
 */
static inline __attribute__((always_inline)) cme_error_t
cme_error_create(int code, const char *file, const char *func, int line,
                 const char *msg) {
  if (!cme_ringbuf)
    return NULL;
  cme_error_t e = &cme_ringbuf[cme_next_idx()];
  e->code = (uint8_t)code;
  e->msg = msg;
  e->frames_length = 1;
  e->frames[0] = (struct cme_Frame){file, func, (uint32_t)line};
  return e;
}

/**
 * Create a formatted error (like printf).
 */
static inline __attribute__((always_inline)) cme_error_t
cme_error_create_fmt(int code, const char *file, const char *func, int line,
                     const char *fmt, ...) {
  if (!cme_ringbuf)
    return NULL;
  cme_error_t e = &cme_ringbuf[cme_next_idx()];
  e->code = (uint8_t)code;
  e->frames_length = 1;
  e->frames[0] = (struct cme_Frame){file, func, (uint32_t)line};

  va_list args;
  va_start(args, fmt);
  vsnprintf(e->_msg_buf, CME_STR_MAX, fmt, args);
  va_end(args);

  e->msg = e->_msg_buf;
  return e;
}

/**
 * Push a backtrace frame onto an existing error.
 */
static inline __attribute__((always_inline)) cme_error_t
cme_error_push_symbol(cme_error_t err, const char *file, const char *func,
                      int line) {
#ifdef CME_ENABLE_BACKTRACE
  if (err && err->frames_length < CME_STACK_MAX)
    err->frames[err->frames_length++] =
        (struct cme_Frame){file, func, (uint32_t)line};
#else
  (void)file;
  (void)func;
  (void)line;
#endif
  return err;
}

/******************************************************************************
 *                             Dump Utilities                                 *
 ******************************************************************************/
/**
 * Convert an error to a formatted string.
 */
static inline int cme_error_dump_to_str(cme_error_t err, uint32_t n,
                                        char *buffer) {
  if (!err || !buffer || n == 0)
    return EINVAL;

  size_t offset = 0;
  int written;

  written = snprintf(buffer + offset, n - offset,
                     "====== ERROR DUMP ======\n"
                     "Error code: %d\n"
                     "Error message: %s\n",
                     err->code, err->msg);
  if (written < 0 || (size_t)written >= n - offset)
    return ENOBUFS;
  offset += (size_t)written;

  written = snprintf(buffer + offset, n - offset, "------------------------\n");
  if (written < 0 || (size_t)written >= n - offset)
    return ENOBUFS;
  offset += (size_t)written;

  for (size_t i = 0; i < err->frames_length; i++) {
    struct cme_Frame *f = &err->frames[i];
    written =
        snprintf(buffer + offset, n - offset, "%u:%s:%s:%u\n", (unsigned)i,
                 f->func ? f->func : "??", f->file ? f->file : "??", f->line);
    if (written < 0 || (size_t)written >= n - offset)
      return ENOBUFS;
    offset += (size_t)written;
  }

  return 0;
}

/**
 * Write an error trace to a file.
 */
static inline int cme_error_dump_to_file(cme_error_t err, char *path) {
  FILE *file = fopen(path, "w");
  if (!file)
    return errno;

  char buffer[4096];
  if (cme_error_dump_to_str(err, sizeof(buffer), buffer) != 0) {
    fclose(file);
    return ENOBUFS;
  }

  fwrite(buffer, 1, strlen(buffer), file);
  fclose(file);
  return 0;
}

/**
 * Reserved for API symmetry.
 */
static inline void cme_error_destroy(cme_error_t err) { (void)err; }

/******************************************************************************
 *                             Convenience Macros                             *
 ******************************************************************************/
/**
 * Create an error with literal string.
 */
#define cme_error(code, msg)                                                   \
  cme_error_create((code), __FILE__, __func__, __LINE__, (msg))

/**
 * Create an error with printf-style formatting.
 */
#define cme_errorf(code, fmt, ...)                                             \
  cme_error_create_fmt((code), __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

/**
 * Propagate an error and add caller frame.
 */
#define cme_return(ERR)                                                        \
  cme_error_push_symbol((ERR), __FILE__, __func__, __LINE__)

#endif // C_MINILIB_ERROR_H
