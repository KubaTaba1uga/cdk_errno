#include <stdio.h>

#include "example_2_lib.h"

/**
 * Return str on success and NULL on failure.
 * On failure sets cdk_errno.
 */
const char *nested_failing_func(void) {
  if (1 > 0) {
    cdk_errno = cdk_errnos(ENOBUFS, "My error");
    return NULL;
  }

  return "All good";
}

/**
 * Return positive int on success and negative int on failure.
 * On failure sets cdk_errno.
 */
int failing_func(void) {
  const char *str = nested_failing_func();
  if (!str) {
    return cdk_ereturn(-1);
  }

  return 13;
}

/**
 * On failure sets cdk_errno.
 */
void func(void) {
  int res = failing_func();
  if (res < 0) {
    cdk_ewrap();
    return;
  }
}

int main(void) {
  char buffer[1024];

  cdk_errno = 0;
  func();
  if (cdk_errno) {
    cdk_edumps(sizeof(buffer), buffer);
    printf("%s", buffer);
    return -1;
  }

  return 0;
}
