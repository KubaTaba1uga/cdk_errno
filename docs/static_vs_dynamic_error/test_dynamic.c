#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "dynamic.h"
#include "levels_dynamic.h"

#define USAGE_MSG                                                              \
  "Usage: %s --max=N --batch=B\n"                                              \
  "  --max     Total number of errors to allocate\n"                           \
  "  --batch   Free every B errors (for immediate free, use --batch==--max)\n"

static double run_benchmark(cme_dynamic_error_t (*fn)(int), int max, int batch) {
  long long start_ns = cme_now_ns();

  for (int i = 0; i < max; i += batch) {
    int current_batch = (i + batch <= max) ? batch : (max - i);
    cme_dynamic_error_t *errors =
        malloc(sizeof(cme_dynamic_error_t) * current_batch);
    if (!errors) {
      perror("malloc failed");
      exit(1);
    }

    for (int j = 0; j < current_batch; ++j) {
      errors[j] = fn(i + j);
    }

    for (int j = 0; j < current_batch; ++j) {
      cme_dynamic_error_t err = errors[j];
      size_t a = strlen(err->msg);
      size_t b = a + strlen(err->source_file);
      size_t c = b + strlen(err->source_func);
      a += c;
    }

    if (i == 0 && fn == some_function_backtrace_dynamic)
      cme_dynamic_error_dump(errors[0], "backtrace_dynamic.txt");
    else if (i == 0 && fn == some_function_frameptr_dynamic)
      cme_dynamic_error_dump(errors[0], "frameptr_dynamic.txt");

    for (int j = 0; j < current_batch; ++j) {
      cme_dynamic_error_destroy(errors[j]);
    }

    free(errors);
  }

  long long end_ns = cme_now_ns();
  return (end_ns - start_ns) / 1e6; // ms
}

int main(int argc, char **argv) {
  int max = 0, batch = 0;

  for (int i = 1; i < argc; ++i) {
    if (strncmp(argv[i], "--max=", 6) == 0)
      max = atoi(argv[i] + 6);
    else if (strncmp(argv[i], "--batch=", 8) == 0)
      batch = atoi(argv[i] + 8);
  }

  if (max <= 0 || batch <= 0 || batch > max) {
    fprintf(stderr, USAGE_MSG, argv[0]);
    return 1;
  }

  printf("Running dynamic error allocation benchmark (frameptr)...\n");
  double time_fp = run_benchmark(some_function_frameptr_dynamic, max, batch);
  printf("Dynamic error allocation test complete:\n");
  printf("  Total errors: %d\n", max);
  printf("  Batch size: %d\n", batch);
  printf("  Bytes per batch: %zu bytes\n",
         sizeof(struct cme_DynamicError) * batch);
  printf("  Time elapsed: %.4f ms\n", time_fp);

  printf("\nRunning dynamic error allocation benchmark (glibc backtrace)...\n");
  double time_bt = run_benchmark(some_function_backtrace_dynamic, max, batch);
  printf("Dynamic error allocation test complete:\n");
  printf("  Total errors: %d\n", max);
  printf("  Batch size: %d\n", batch);
  printf("  Bytes per batch: %zu bytes\n",
         sizeof(struct cme_DynamicError) * batch);
  printf("  Time elapsed: %.4f ms\n", time_bt);

  double delta = (time_bt - time_fp) / time_fp * 100.0;
  printf("\nComparison summary:\n");
  printf("  Δ time (glibc - frameptr): %.1f%%\n", delta);

  return 0;
}
