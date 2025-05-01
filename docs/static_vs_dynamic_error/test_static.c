#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "static.h"

#define USAGE_MSG                                                              \
  "Usage: %s --max=N --batch=B\n"                                              \
  "  --max     Total number of errors to allocate\n"                           \
  "  --batch   Free every B errors (for immediate free, use --batch==--max)\n"

static inline long long now_ns(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

int main(int argc, char **argv) {
  int max = 0;
  int batch = 0;

  // Parse args
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

  long long start_ns = now_ns();

  for (int i = 0; i < max; i += batch) {
    int current_batch = (i + batch <= max) ? batch : (max - i);

    cme_static_error_t *errors = malloc(sizeof(void *) * current_batch);
    if (!errors) {
      perror("malloc failed");
      return 1;
    }

    for (int j = 0; j < current_batch; ++j) {
      errors[j] = cme_static_errorf(123, "error #%d", i + j);
    }

    for (int j = 0; j < current_batch; ++j) {
      cme_static_error_destroy(errors[j]);
    }

    free(errors);
  }

  long long end_ns = now_ns();
  double elapsed_ms = (end_ns - start_ns) / 1e6;

  printf("Static error allocation test complete:\n");
  printf("  Total: %d errors\n", max);
  printf("  Batch size: %d\n", batch);
  printf("  Time elapsed: %.4f ms\n", elapsed_ms);

  return 0;
}
