#include <stdio.h>

#include "example_1_lib.h"

cdk_error_t nested_failing_func(void) {
  if (1 > 0) {
    return errori(ENOBUFS);
  }

  return 0;
}

cdk_error_t failing_func(void) {
  cdk_error_t err = nested_failing_func();
  if (err) {
    return cdk_error_wrap(err);
  }

  return 0;
}

int main(void) {
  char buffer[1024];
  cdk_error_t err;

  if (init_error()) {
    return ENOMEM;
  }

  err = failing_func();

  cdk_error_dumps(err, sizeof(buffer), buffer);
  puts(buffer);

  destroy_error();

  return 0;
}
