#ifndef ERROR_H
#define ERROR_H

#define CDK_DISABLE_ERRNO_API
#include "cdk_error.h"

extern cdk_error_t hidden_error;

int init_error(void);
void destroy_error(void);

#define errori(code) cdk_errori(hidden_error, code)

#define errors(code, msg) cdk_errors(hidden_error, code, msg)

#define errorf(code, fmt, ...)                                                 \
  cdk_errorf(hidden_error, code, fmt, ##__VA_ARGS__)

#define ewrap(ret) cdk_error_wrap(hidden_errno)

#define edumps(buf_size, buf) cdk_error_dumps(hidden_errno, buf_size, buf)

#endif // ERROR_H
