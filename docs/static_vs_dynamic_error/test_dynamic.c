#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "dynamic.h"

#define USAGE_MSG                                                              \
  "Usage: %s --max=N --batch=B\n"                                              \
  "  --max     Total number of errors to allocate\n"                           \
  "  --batch   Free every B errors (for immediate free, use --batch==--max)\n"

int main(int argc, char **argv) {
  int max = 0, batch = 0;

  for (int i = 1; i < argc; ++i) {
    if (strncmp(argv[i], "--max=", 6) == 0) {
      max = atoi(argv[i] + 6);
    } else if (strncmp(argv[i], "--batch=", 8) == 0) {
      batch = atoi(argv[i] + 8);
    }
  }
  if (max <= 0 || batch <= 0 || batch > max) {
    fprintf(stderr, USAGE_MSG, argv[0]);
    return 1;
  }

  long long start_ns = cme_now_ns();

  for (int i = 0; i < max; i += batch) {
    int current_batch = (i + batch <= max) ? batch : (max - i);
    cme_dynamic_error_t *errors = malloc(sizeof(void *) * current_batch);
    if (!errors) {
      perror("malloc failed");
      return 1;
    }
    for (int j = 0; j < current_batch; ++j) {
      errors[j] = cme_dynamic_errorf(123, "error #%d", i + j);
    }

    // We need to touch values to elevate cpu hot cache, if you comment this
    //  out you will see how much touching defragmented memory costs.
    for (int j = 0; j < current_batch; ++j) {
      cme_dynamic_error_t err = errors[j];
      size_t a = strlen(err->msg);
      size_t b = a + strlen(err->source_file);
      size_t c = b + strlen(err->source_func);
      a += c;
    }

    for (int j = 0; j < current_batch; ++j) {
      cme_dynamic_error_destroy(errors[j]);
    }

    free(errors);
  }

  long long end_ns = cme_now_ns();
  long long elapsed_ns = (end_ns - start_ns);

  printf("Dynamic error allocation test complete:\n");
  printf("  Total: %d errors\n", max);
  printf("  Batch size: %d\n", batch);
  printf("  Bytes per batch: %zu bytes\n",
         sizeof(struct cme_DynamicError) * batch);
  printf("  Time elapsed:  %llds\n", elapsed_ns);
  return 0;
}
