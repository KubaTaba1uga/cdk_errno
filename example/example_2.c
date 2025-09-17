#include <stdio.h>

#include "cdk_error.h"

const char *nested_failing_func(void) {
  if (1 > 0) {
    cdk_errno = cdk_errnos(ENOBUFS, "My error");
    return NULL;
  }

  return "All good";
}

int failing_func(void) {
  const char *str = nested_failing_func();
  if (!str) {
    return cdk_ereturn(13);
  }

  return 13;
}

int main(void) {
  char buffer[1024];

  cdk_errno = 0;
  int result = failing_func();
  if (cdk_errno) {
    cdk_edumps(sizeof(buffer), buffer);
    printf("%s", buffer);
    return -1;
  }

  printf("Result is %d", result);

  return 0;
}
