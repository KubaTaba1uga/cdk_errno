/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef CDK_ERROR_H
#define CDK_ERROR_H

#include <stddef.h>
#ifdef __STDC_NO_THREADS__
#error "Threads extension is required to compile this library"
#endif

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>

//
////
//////
/******************************************************************************
   C Development Kit: Error - Modern errno mechanism.

   This library provides an errno alaike error mechanism but with more advanced
   error structure than pure integer. The goal is to mimic errno but allow more
   fields and more advanced error features than errno allows currently. All
   fields beside integer are optional and configurable making the library highly
   customizable.

   To use integer only error:
     - cdk_error(uint16_t code)

   If you want to return error from function good practice is to use cdk_ereturn
   function. Using cdk_ereturn allow you to gather backtrace, if you are not
   interested in backtrace you can skip cdk_ereturn function.

   String Literal:
    To allow cdk_error_t to store string literals you need set
    CDK_ERROR_LSTR_ENABLE to 1. Once feature is enabled these functions become
    available:
      - cdk_errors(uint16_t code, const char *msg)

    To get string literal use cdk_error_t->msg field.

   Formatted string:
    To allow cdk_error_t to store formatted string alike `sprintf` function you
    need to set CDK_ERROR_FSTR_ENABLE to 1. Once feature is enabled these
    functions become available:
      - cdk_errorf(uint16_t code, const char *fmt, ...)

    You can customize the size of buffer available in cdk_error_t struct via
    CDK_ERROR_FSTR_MAX macro.

    To get formatted string use cdk_error_t->msg field.

   Backtrace:
    To allow cdk_error_t to store backtrace of the error you need to set
    CDK_ERROR_BTRACE_ENABLE to 1. Once feature is enabled these functions change
    behaviour to enable backtrace gathering:
     - cdk_error(uint16_t code)
     - cdk_errors(uint16_t code, const char *msg)
     - cdk_errorf(uint16_t code, const char *fmt, ...)

    To get backtrace frame use cdk_error_t->btraces and cdk_error_t->btraces_len
    fields.

    To gather backtrace you need to use cdk_ereturn function or cdk_ewrap
    function it depends how you aproach returning an error. If you signal error
    by returning some error value from the function you should use cdk_ereturn,
    on the other hand if you signal the error by clearing and setting cdk_errno
   you should use cdk_ewrap function.

 ******************************************************************************/
//////////

/******************************************************************************
 *                             Config / Limits                                *
 ******************************************************************************/
#ifndef CDK_ERROR_LSTR_ENABLE
#define CDK_ERROR_LSTR_ENABLE 1
#endif

#ifndef CDK_ERROR_FSTR_ENABLE
#define CDK_ERROR_FSTR_ENABLE 1
#endif

#ifndef CDK_ERROR_FSTR_MAX
#define CDK_ERROR_FSTR_MAX 1024
#endif

#ifndef CDK_ERROR_BTRACE_ENABLE
#define CDK_ERROR_BTRACE_ENABLE 1
#endif

#ifndef CDK_ERROR_BTRACE_MAX
#define CDK_ERROR_BTRACE_MAX 32
#endif

/******************************************************************************
 *                             Data types                                     *
 ******************************************************************************/
/**
 * Backtrace frame (file/function/line tuple).
 */
struct cdk_BTrace {
  const char *file;
  const char *func;
  uint32_t line;
};

/**
 * Error object.
 */
struct cdk_Error {
  uint16_t code;

#if CDK_ERROR_LSTR_ENABLE == 1 || CDK_ERROR_FSTR_ENABLE == 1
  const char *msg;
#endif

#if CDK_ERROR_FSTR_ENABLE == 1
  char _msg_buf[CDK_ERROR_FSTR_MAX];
#endif

#if CDK_ERROR_BTRACE_ENABLE == 1
  struct cdk_BTrace btraces[CDK_ERROR_BTRACE_MAX];
  size_t btraces_len;
#endif
};

/**
 * Typedef to save some typing.
 */
typedef struct cdk_Error *cdk_error_t;

/******************************************************************************
 *                                 API                                        *
 ******************************************************************************/
/**
 * `error` equivalent. It should be set to 0, before usage
 *   and checked if not 0 after function execution.
 */
thread_local extern struct cdk_Error *cdk_errno;

/**
 * Private storage for underlying cdk_error. You shouldn't need
 *   to touch this variable.
 */
thread_local extern struct cdk_Error _cdk_thread_error;

/**
 * Create new error. The value created by this function should
 *   be used to set cdk_error.
 */
#if CDK_ERROR_BTRACE_ENABLE == 1
#define cdk_error(code) _cdk_error((code), __FILE__, __func__, __LINE__)
static inline cdk_error_t _cdk_error(uint16_t code, const char *file,
                                     const char *func, int line) {
  _cdk_thread_error = (struct cdk_Error){
      .code = code,
      .btraces = {{.file = file, .func = func, .line = line}},
      .btraces_len = 1,
  };

  return &_cdk_thread_error;
}
#else
#define cdk_error(code) _cdk_error((code))
static inline cdk_error_t _cdk_error(uint16_t code) {
  _cdk_thread_error = (struct cdk_Error){
      .code = code,
  };

  return &_cdk_thread_error;
};
#endif

/**
 * Create new error using integer and string literal. The value created by this
 *   function should be used to set cdk_error.
 */
#if CDK_ERROR_LSTR_ENABLE == 1
#if CDK_ERROR_BTRACE_ENABLE == 1
#define cdk_errors(code, msg)                                                  \
  _cdk_errors((code), (msg), __FILE__, __func__, __LINE__)
static inline cdk_error_t _cdk_errors(uint16_t code, const char *msg,
                                      const char *file, const char *func,
                                      int line) {
  _cdk_thread_error = (struct cdk_Error){
      .code = code,
      .msg = msg,
      .btraces = {{.file = file, .func = func, .line = line}},
      .btraces_len = 1,
  };

  return &_cdk_thread_error;
}
#else
#define cdk_errors(code, msg) _cdk_errors((code), (msg))
static inline struct cdk_Error *_cdk_errors(uint16_t code, const char *msg) {
  _cdk_thread_error = (struct cdk_Error){
      .code = code,
      .msg = msg,
  };

  return &_cdk_thread_error;
}
#endif
#endif

/**
 * Create new error using integer and string formatted values. The value created
 *   by this function should be used to set cdk_error. Watch out for assert
 *   inside of this function, it will abort the program execution if error's
 *   internal buffer overflow occured. If that's the case try trimming during
 *   formatting to not allow for buffer overflow:
 *     const char *str = "Hello, world!";
 *     printf("%.03s and %.05s\n", str, str);       // prints "Hel and Hello"
 *     printf("%.*s and %.*s\n", 3, str, 5, str);   // prints "Hel and Hello"
 *
 *    It works exactly the same with cdk_errorf function. So you can trimm
 *    the output without additional operations
 *     const char *str = "Hello, world!";
 *     cdk_errorf(EINVAL, "%.03s and %.05s\n", str, str);  // prints:
 *                                                         //  "Hel and Hello"
 *
 */
#if CDK_ERROR_FSTR_ENABLE == 1
#if CDK_ERROR_BTRACE_ENABLE == 1
#define cdk_errorf(code, fmt, ...)                                             \
  _cdk_errorf((code), __FILE__, __func__, __LINE__, (fmt), ##__VA_ARGS__)
static inline struct cdk_Error *_cdk_errorf(uint16_t code, const char *file,
                                            const char *func, int line,
                                            const char *fmt, ...) {
  _cdk_thread_error = (struct cdk_Error){
      .code = code,
      .btraces = {{.file = file, .func = func, .line = line}},
      .btraces_len = 1,
  };

  va_list args;
  va_start(args, fmt);
  int written_bytes = vsnprintf(_cdk_thread_error._msg_buf,
                                sizeof(_cdk_thread_error._msg_buf), fmt, args);
  va_end(args);

  assert(written_bytes >= 0);

  _cdk_thread_error.msg = _cdk_thread_error._msg_buf;

  return &_cdk_thread_error;
}
#else
#define cdk_errorf(code, fmt, ...) _cdk_errorf((code), (fmt), ##__VA_ARGS__)
static inline struct cdk_Error *_cdk_errorf(uint16_t code, const char *fmt,
                                            ...) {
  _cdk_thread_error = (struct cdk_Error){
      .code = code,
  };

  va_list args;
  va_start(args, fmt);
  int written_bytes = vsnprintf(_cdk_thread_error._msg_buf,
                                sizeof(_cdk_thread_error._msg_buf), fmt, args);
  va_end(args);

  assert(written_bytes >= 0);

  _cdk_thread_error.msg = _cdk_thread_error._msg_buf;

  return &_cdk_thread_error;
}
#endif
#endif

static inline void cdk_error_dump_to_str(cdk_error_t err, size_t buf_size,
                                         char *buf) {
  if (!err || !buf || buf_size == 0) {
    return;
  }

  size_t offset = 0;
  int written;

  written = snprintf(buf + offset, buf_size - offset,
                     "====== ERROR DUMP ======\n"
                     "Error code: %s\n",
                     strerror(err->code));
  if (written < 0 || (size_t)written >= buf_size - offset) {
    return;
  }
  offset += (size_t)written;

#if CDK_ERROR_FSTR_ENABLE == 1 || CDK_ERROR_LSTR_ENABLE == 1
  written = snprintf(buf + offset, buf_size - offset, "Error message: %.*s\n",
                     CDK_ERROR_FSTR_MAX, err->msg);
  if (written < 0 || (size_t)written >= buf_size - offset) {
    return;
  }
  offset += (size_t)written;
#endif

  written =
      snprintf(buf + offset, buf_size - offset, "------------------------\n");
  if (written < 0 || (size_t)written >= buf_size - offset) {
    return;
  }
  offset += (size_t)written;
};

#if CDK_ERROR_BTRACE_ENABLE == 1
#define cdk_ewrap(err)                                                         \
  err->btraces[err->btraces_len++] = (struct cdk_BTrace) {                     \
    .file = __FILE__, .func = __func__, .line = __LINE__                       \
  }

#define cdk_ereturn(ret, err)                                                  \
  ({                                                                           \
    cdk_ewrap(err);                                                            \
    ret;                                                                       \
  })
#endif

#endif // CDK_ERROR_H
