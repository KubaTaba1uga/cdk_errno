/*
 * Copyright (c) 2025 Jakub Buczynski <KubaTaba1uga>
 * SPDX-License-Identifier: MIT
 * See LICENSE file in the project root for full license information.
 */

#ifndef CDK_ERRNO_H
#define CDK_ERRNO_H

#ifdef __STDC_NO_THREADS__
#errno "Threads extension is required to compile this library"
#endif

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <threads.h>

//
////
//////
/******************************************************************************
   C Development Kit: Errno - Modern errno mechanism.

   This library provides an errno alaike error mechanism but with more advanced
   errno structure than pure integer. The goal is to mimic errno but allow more
   fields and more advanced error features than errno allows currently. All
   fields beside integer are optional and configurable making the library highly
   customizable.

   String Literal:
    To allow cdk_errno_t to store string literals you need set
    CDK_ERRNO_LSTR_ENABLE to 1. Once feature is enabled this functions become
    available:
      - cdk_errors(uint16_t code, const char *msg)

    To get string literal use cdk_errno_t->msg field.

   Formatted string:
    To allow cdk_errno_t to store formatted string alike `sprintf` function you
    need to set CDK_ERRNO_FSTR_ENABLE to 1. Once feature is enabled this
    functions become available:
      - cdk_errorf(uint16_t code, const char *fmt, ...)

    You can customize the size of buffer available in cdk_errno_t struct via
    CDK_ERRNO_FSTR_MAX macro.

    To get formatted string use cdk_errno_t->msg field.


 ******************************************************************************/
//////////

/******************************************************************************
 *                             Config / Limits                                *
 ******************************************************************************/
#ifndef CDK_ERRNO_LSTR_ENABLE
#define CDK_ERRNO_LSTR_ENABLE 0
#endif

#ifndef CDK_ERRNO_FSTR_ENABLE
#define CDK_ERRNO_FSTR_ENABLE 0
#endif

#ifndef CDK_ERRNO_FSTR_MAX
#define CDK_ERRNO_FSTR_MAX 1024
#endif

/******************************************************************************
 *                             Data types                                     *
 ******************************************************************************/
struct cdk_Errno {
  uint16_t code;

#if CDK_ERRNO_LSTR_ENABLE == 1 || CDK_ERRNO_FSTR_ENABLE == 1
  const char *msg;
#endif

#if CDK_ERRNO_FSTR_ENABLE == 1
  char _msg_buf[CDK_ERRNO_FSTR_MAX];
#endif
};

typedef struct cdk_Errno *cdk_errno_t;

/******************************************************************************
 *                                 API                                        *
 ******************************************************************************/
/**
 * `errno` equivalent. It should be set to 0, before usage
 *   and checked if not 0 after function execution.
 */
thread_local extern struct cdk_Errno *cdk_errno;

/**
 * Private storage for underlying cdk_errno. You shouldn't need
 *   to touch this variable.
 */
thread_local extern struct cdk_Errno cdk_thread_error;

/**
 * Create new errno. The value created by this function should
 *   be used to set cdk_errno.
 */
static inline struct cdk_Errno *cdk_error(uint16_t code) {
  cdk_thread_error = (struct cdk_Errno){
      .code = code,
  };

  return &cdk_thread_error;
};

#if CDK_ERRNO_LSTR_ENABLE == 1
/**
 * Create new errno using integer and string literal. The value created by this
 *   function should be used to set cdk_errno.
 */
static inline struct cdk_Errno *cdk_errors(uint16_t code, const char *msg) {
  cdk_thread_error = (struct cdk_Errno){
      .code = code,
      .msg = msg,
  };

  return &cdk_thread_error;
}
#endif

#if CDK_ERRNO_FSTR_ENABLE == 1
/**
 * Create new errno using integer and string formatted values. The value created
 *   by this function should be used to set cdk_errno. Watch out for assert
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
static inline struct cdk_Errno *cdk_errorf(uint16_t code, const char *fmt,
                                           ...) {
  cdk_thread_error = (struct cdk_Errno){
      .code = code,
  };

  va_list args;
  va_start(args, fmt);
  int written_bytes = vsnprintf(cdk_thread_error._msg_buf,
                                sizeof(cdk_thread_error._msg_buf), fmt, args);
  va_end(args);

  assert(written_bytes >= 0);

  cdk_thread_error.msg = cdk_thread_error._msg_buf;

  return &cdk_thread_error;
}
#endif

#endif // CDK_ERRNO_H
