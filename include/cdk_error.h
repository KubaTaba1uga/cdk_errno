/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef CDK_ERROR_H
#define CDK_ERROR_H

#include <__stdarg_va_list.h>
#ifdef __STDC_NO_THREADS__
#error "Threads extension is required to compile this library"
#endif

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>

//
////
//////
/******************************************************************************
   C Development Kit: Error - Modern errno mechanism.

   TO-DO: write usage comment
 ******************************************************************************/
//////////

/******************************************************************************
 *                             Config / Limits                                *
 ******************************************************************************/
#ifndef CDK_ERROR_FSTR_MAX
#define CDK_ERROR_FSTR_MAX 1024
#endif

#ifndef CDK_ERROR_BTRACE_MAX
#define CDK_ERROR_BTRACE_MAX 32
#endif

/******************************************************************************
 *                             Data types                                     *
 ******************************************************************************/
enum cdk_ErrorType {
  cdk_ErrorType_INT,
  cdk_ErrorType_STR,
  cdk_ErrorType_FSTR,
};

struct cdk_EFrame {
  const char *file;
  const char *func;
  uint16_t line;
};

/**
 * Common error object.
 */
struct cdk_Error {
  // Generic error
  enum cdk_ErrorType type;
  uint16_t code;

  // String and formatted string
  const char *msg;
  char _msg_buf[CDK_ERROR_FSTR_MAX];

  // Backtrace
  struct cdk_EFrame eframes[CDK_ERROR_BTRACE_MAX];
  size_t eframes_len;
};

typedef struct cdk_Error *cdk_error_t;

/******************************************************************************
 *                                 Generic API                                *
 ******************************************************************************/
/**
 * Create struct cdk_Error.
 */
static inline cdk_error_t cdk_error_int(struct cdk_Error *err, uint16_t code,
                                        const char *file, const char *func,
                                        int line) {
  *err = (struct cdk_Error){
      .type = cdk_ErrorType_INT,
      .code = code,
      .eframes = {{.file = file, .func = func, .line = line}},
      .eframes_len = 1,
  };

  return err;
};

/**
 * Create struct cdk_StrError.
 */
static inline cdk_error_t cdk_error_lstr(struct cdk_Error *err, uint16_t code,
                                         const char *file, const char *func,
                                         int line, const char *msg) {
  *err = (struct cdk_Error){
      .type = cdk_ErrorType_STR,
      .code = code,
      .msg = msg,
      .eframes = {{.file = file, .func = func, .line = line}},
      .eframes_len = 1,
  };

  return err;
};

/**
 * Create struct cdk_StrFmtError.
 */
static inline cdk_error_t cdk_error_fstr(struct cdk_Error *err, uint16_t code,
                                         const char *file, const char *func,
                                         int line, const char *fmt, ...) {

  *err = (struct cdk_Error){
      .type = cdk_ErrorType_FSTR,
      .code = code,
      .eframes = {{.file = file, .func = func, .line = line}},
      .eframes_len = 1,
  };

  va_list args;
  va_start(args, fmt);
  int written_bytes =
      vsnprintf(err->_msg_buf, sizeof(err->_msg_buf), fmt, args);

  assert(written_bytes >= 0);

  err->msg = err->_msg_buf;

  return err;
};

/**
 * Dump all struct cdk_XError to string.
 */
static inline int cdk_error_dumps(cdk_error_t err, size_t buf_size, char *buf) {
  size_t offset = 0;
  int written;

  written = snprintf(buf, buf_size,
                     "====== ERROR DUMP ======\n"
                     "Error code: %s\n",
                     strerror(err->code));
  if (written < 0 || (size_t)written >= buf_size) {
    return ENOBUFS;
  }
  offset += written;

  switch (err->type) {
  case cdk_ErrorType_STR:
    written =
        snprintf(buf + offset, buf_size - offset, "Error msg: %s\n", err->msg);
    if (written < 0 || (size_t)written >= buf_size) {
      return ENOBUFS;
    }
    offset += written;
    break;

  case cdk_ErrorType_FSTR:
    written = snprintf(buf + offset, buf_size - offset, "Error msg: %.*s\n",
                       (int)sizeof(err->_msg_buf), err->msg);
    if (written < 0 || (size_t)written >= buf_size) {
      return ENOBUFS;
    }
    offset += written;
    break;

  default:;
  }

  written =
      snprintf(buf + offset, buf_size - offset, "------------------------\n");
  if (written < 0 || (size_t)written >= buf_size) {
    return ENOBUFS;
  }
  offset += written;

  return 0;
}

#define cdk_error_wrap(err)                                                    \
  err->eframes[err->eframes_len++] = (struct cdk_EFrame) {                     \
    .file = __FILE__, .func = __func__, .line = __LINE__                       \
  }

#define cdk_error_return(ret, err)                                             \
  ({                                                                           \
    cdk_ewrap(err);                                                            \
    ret;                                                                       \
  })

/******************************************************************************
 *                                >=C11 API                                   *
 ******************************************************************************/
thread_local extern cdk_error_t cdk_errno;
thread_local extern struct cdk_Error cdk_hidden_errno;

static inline cdk_error_t cdk_errori(uint16_t code) {
  return cdk_error_int(&cdk_hidden_errno, code, __FILE_NAME__, __func__,
                       __LINE__);
}

static inline cdk_error_t cdk_errors(uint16_t code, const char *msg) {
  return cdk_error_lstr(&cdk_hidden_errno, code, __FILE_NAME__, __func__,
                        __LINE__, msg);
}

#define cdk_errorf(code, fmt, ...)                                             \
  cdk_error_fstr(&cdk_hidden_errno, (code), __FILE_NAME__, __func__, __LINE__, \
                 (fmt), ##__VA_ARGS__)

#define cdk_ewrap() cdk_error_wrap(&cdk_errno)
#define cdk_ereturn(ret)                                                       \
  ({                                                                           \
    cdk_ewrap();                                                               \
    ret;                                                                       \
  })

#endif // CDK_ERROR_H
