#include "cdk_error.h"
#include <stdio.h>

struct cdk_Error error;

cdk_error_t nested_failing_func(void) {
  return cdk_error_lstr(&error, ENOBUFS, "Buffer to small");
}

cdk_error_t failing_func(void) { return nested_failing_func(); }

int main(void) {
  char buffer[1024];
  cdk_error_t err;
  err = nested_failing_func();

  cdk_error_dumps(err, sizeof(buffer), buffer);

  puts(buffer);

  return 0;
}
