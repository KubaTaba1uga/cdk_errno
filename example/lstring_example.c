#include "copy.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define CDK_ERRNO_LSTR_ENABLE 1
#include "cdk_error.h"

int divide(int a, int b) {
  if (b == 0) {
    /* printf("%lu\n\n\n", sizeof(struct cdk_Errno));     */
    cdk_errno = cdk_errors(EINVAL, "Cannot divide by 0");
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
