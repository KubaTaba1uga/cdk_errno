#include <stdlib.h>

#include "example_1_lib.h"

cdk_error_t hidden_error = NULL;

int init_error(void) {
  if (hidden_error) {
    return 0;
  }

  hidden_error = malloc(sizeof(struct cdk_Error));
  if (!hidden_error) {
    return ENOMEM;
  }

  return 0;
}

void destroy_error(void) {
  if (!hidden_error) {
    return;
  }

  free(hidden_error);
  hidden_error = NULL;
}
