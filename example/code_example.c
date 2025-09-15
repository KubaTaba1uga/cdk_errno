#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "cdk_error.h"

int divide(int a, int b) {
  if (b == 0) {
    cdk_errno = cdk_error(EINVAL);
    return 0;
  }

  return a / b;
}

int main(void) {
  cdk_errno = 0;
  int result = divide(5, 0);
  if (cdk_errno) {
    printf("Detected error: %s", strerror(cdk_errno->code));
    return 1;
  }

  printf("Result: %d", result);

  return 0;
}
