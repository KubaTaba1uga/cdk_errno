#include "cdk_error.h"
#include <stdio.h>

thread_local cdk_error_t cdk_errno;
thread_local struct cdk_Error cdk_hidden_errno;

int nested_failing_func(void) {
  cdk_errno = cdk_errors(ENOBUFS, "Buffer to small");

  return -1;
}

int failing_func(void) {
  if (nested_failing_func() < 0) {
    return cdk_ereturn(-1);
  };

  return 0;
}

int main(void) {
  char buffer[1024];

  cdk_errno = 0;
  nested_failing_func();
  if (cdk_errno) {
    cdk_error_dumps(cdk_errno, sizeof(buffer), buffer);
    puts(buffer);
  }

  return 0;
}
