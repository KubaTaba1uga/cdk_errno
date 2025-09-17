/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef CDK_ERROR_H
#define CDK_ERROR_H

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
 C Development Kit: Error – a modern, fast, errno-like error mechanism.

 Purpose:
   Provide an errno-style mechanism with extra context:
   - numeric error code (compatible with <errno.h>)
   - optional message (literal or formatted)
   - lightweight manual backtrace (file, function, line)

 Features:
   - No heap allocations, all storage is local or thread-local.
   - Three error types:
       INT  – code only, fastest
       STR  – code + literal string
       FSTR – code + formatted string (disabled if CDK_ERROR_OPTIMIZE defined)
   - Thread-local errno analogue (cdk_errno) (disabled if CDK_DISABLE_ERRNO_API
     defined).
   - Manual backtrace via macros (no platform unwinding needed).

 Usage (local error object):
   struct cdk_Error err;
   return cdk_errori(&err, EINVAL);             // code only
   return cdk_errors(&err, ENOENT, "not found");// literal message
   return cdk_errorf(&err, EIO, "disk %u", d);  // formatted (if enabled)

   // Add a frame when propagating:
   int res = some_fn();
   if (res){
     cdk_error_return(-1, &err);
   }

   // Print error dump:
   char buf[512];
   cdk_error_dumps(&err, sizeof(buf), buf);
   puts(buf);

 Usage (errno-like thread-local API):
   cdk_errnoi(EAGAIN);
   cdk_errnos(EPERM, "denied");
   cdk_errnof(EBUSY, "port %u", p);
   cdk_ewrap();
   return cdk_ereturn(-1);
   cdk_edumps(sizeof buf, buf);

 Configuration macros:
   CDK_ERROR_BTRACE_MAX   (default 16)  – max backtrace frames
   CDK_ERROR_FSTR_MAX     (default 255) – formatted string buffer
   CDK_ERROR_OPTIMIZE                 – drop FSTR support entirely
   CDK_DISABLE_ERRNO_API              – disable thread-local errno analogue

 Requirements:
   C11 threads support (<threads.h>).
*****************************************************************************/
//////////

/******************************************************************************
 *                             Config / Limits                                *
 ******************************************************************************/
#ifndef CDK_ERROR_FSTR_MAX
#define CDK_ERROR_FSTR_MAX 255
#endif

#ifndef CDK_ERROR_BTRACE_MAX
#define CDK_ERROR_BTRACE_MAX 16
#endif

#ifndef CDK_DISABLE_ERRNO_API
#endif

/******************************************************************************
 *                             Data types *
 ******************************************************************************/
/**
 * Error type.
 */
enum cdk_ErrorType {
  cdk_ErrorType_INT,
  cdk_ErrorType_STR,
#ifndef CDK_ERROR_OPTIMIZE
  cdk_ErrorType_FSTR,
#endif
};

/**
 * Error frame object.
 */
struct cdk_EFrame {
  const char *file;
  const char *func;
  uint32_t line;
};

/**
 * Common error object.
 */
struct cdk_Error {
  enum cdk_ErrorType type;                         // Error type
  uint16_t code;                                   // Status code
  const char *msg;                                 // String msg, can be NULL
  struct cdk_EFrame eframes[CDK_ERROR_BTRACE_MAX]; // Backtrace frames
  size_t eframes_len;                              // Backtrace frames length

#ifndef CDK_ERROR_OPTIMIZE
  char _msg_buf[CDK_ERROR_FSTR_MAX]; // Internal storage for formatted string
#endif
};

typedef struct cdk_Error *cdk_error_t;

/******************************************************************************
 *                                 Generic API                                *
 ******************************************************************************/
/**
 * Create struct cdk_Error of type cdk_ErrorType_INT.
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
 * Create struct cdk_Error of type cdk_ErrorType_STR.
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

#ifndef CDK_ERROR_OPTIMIZE
/**
 * Create struct cdk_Error of type cdk_ErrorType_FSTR.
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
  va_end(args);

  assert(written_bytes >= 0);

  err->msg = err->_msg_buf;

  return err;
};
#endif

/**
 * Dump all struct cdk_XError to string.
 */
static inline int cdk_error_dumps(cdk_error_t err, size_t buf_size, char *buf) {
  size_t offset = 0;
  int written;

  written = snprintf(buf, buf_size,
                     "====== ERROR DUMP ======\n"
                     "Error code: %d\n"
                     "Error desc: %s\n",
                     err->code, strerror(err->code));
  if (written < 0 || (size_t)written >= buf_size) {
    return ENOBUFS;
  }
  offset += written;

  if (err->type > cdk_ErrorType_INT) {
    written =
        snprintf(buf + offset, buf_size - offset, "------------------------\n");
    if (written < 0 || (size_t)written >= buf_size) {
      return ENOBUFS;
    }
    offset += written;
  }

  switch (err->type) {
  case cdk_ErrorType_STR:
    written =
        snprintf(buf + offset, buf_size - offset, " Error msg: %s\n", err->msg);
    if (written < 0 || (size_t)written >= buf_size - offset) {
      return ENOBUFS;
    }
    offset += written;
    break;

#ifndef CDK_ERROR_OPTIMIZE
  case cdk_ErrorType_FSTR:
    written = snprintf(buf + offset, buf_size - offset, " Error msg: %.*s\n",
                       (int)sizeof(err->_msg_buf), err->msg);
    if (written < 0 || (size_t)written >= buf_size - offset) {
      return ENOBUFS;
    }
    offset += written;
    break;
#endif
  default:;
  }

  written =
      snprintf(buf + offset, buf_size - offset, "------------------------\n");
  if (written < 0 || (size_t)written >= buf_size - offset) {
    return ENOBUFS;
  }
  offset += written;

  written = snprintf(buf + offset, buf_size - offset, " Backtrace:\n");
  if (written < 0 || (size_t)written >= buf_size - offset) {
    return ENOBUFS;
  }
  offset += written;

  for (int i = 0; i < err->eframes_len; i++) {
    written = snprintf(buf + offset, buf_size - offset, "   [%02d] %s:%s:%d\n",
                       i, err->eframes[i].file, err->eframes[i].func,
                       err->eframes[i].line);
    if (written < 0 || (size_t)written >= buf_size - offset) {
      return ENOBUFS;
    }
    offset += written;
  }

  return 0;
}
static inline void cdk_error_add_frame(cdk_error_t err,
                                       struct cdk_EFrame *frame) {
  if (err->eframes_len >= CDK_ERROR_BTRACE_MAX) {
    return;
  }
  err->eframes[err->eframes_len++] = *frame;
}

#define cdk_error_wrap(err)                                                    \
  ({                                                                           \
    cdk_error_add_frame(err, &(struct cdk_EFrame){.file = __FILE_NAME__,       \
                                                  .func = __func__,            \
                                                  .line = __LINE__});          \
    err;                                                                       \
  })
#define cdk_error_return(ret, err)                                             \
  ({                                                                           \
    cdk_error_wrap(err);                                                       \
    ret;                                                                       \
  })

#define cdk_errori(err, code)                                                  \
  cdk_error_int((err), (code), __FILE_NAME__, __func__, __LINE__)

#define cdk_errors(err, code, msg)                                             \
  cdk_error_lstr((err), (code), __FILE_NAME__, __func__, __LINE__, (msg))

#define cdk_errorf(err, code, fmt, ...)                                        \
  cdk_error_fstr((err), (code), __FILE_NAME__, __func__, __LINE__, (fmt),      \
                 ##__VA_ARGS__)

/******************************************************************************
 *                                Errno API                                   *
 ******************************************************************************/
#ifndef CDK_DISABLE_ERRNO_API
_Thread_local extern cdk_error_t cdk_errno;
_Thread_local extern struct cdk_Error cdk_hidden_errno;

#define cdk_errnoi(code) cdk_errori(&cdk_hidden_errno, code)

#define cdk_errnos(code, msg) cdk_errors(&cdk_hidden_errno, code, msg)

#ifndef CDK_ERROR_OPTIMIZE
#define cdk_errnof(code, fmt, ...)                                             \
  cdk_errorf(&cdk_hidden_errno, code, fmt, ##__VA_ARGS__)
#endif

#define cdk_ewrap() cdk_error_wrap(&cdk_hidden_errno)

#define cdk_ereturn(ret) cdk_error_return((ret), &cdk_hidden_errno)

#define cdk_edumps(buf_size, buf)                                              \
  cdk_error_dumps(&cdk_hidden_errno, buf_size, buf)
#endif

#endif // CDK_ERROR_H
