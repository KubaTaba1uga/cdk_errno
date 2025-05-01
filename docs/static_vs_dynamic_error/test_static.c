#include <stddef.h>
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "static.h"

#define USAGE_MSG                                                              \
  "Usage: %s --max=N --batch=B\n"                                              \
  "  --max     Total number of errors to allocate\n"                           \
  "  --batch   Free every B errors (for immediate free, use --batch==--max)\n"

// Propably better write something like return cme(err)
__attribute__((noinline)) static cme_static_error_t level3(int i) {
  cme_static_error_t err = cme_static_errorf(123, "err %d", i);
  cme_return(err);
}

__attribute__((noinline)) static cme_static_error_t level2(int i) {
  cme_static_error_t err = level3(i);
  cme_return(err);
}

__attribute__((noinline)) static cme_static_error_t level1(int i) {
  cme_static_error_t err = level2(i);
  cme_return(err);
}

__attribute__((noinline)) static cme_static_error_t some_function(int i) {
  cme_static_error_t err = level1(i);
  cme_return(err);
}

int main(int argc, char **argv) {
  int max = 0;
  int batch = 0;

  // Parse command-line arguments
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

    // Allocate array of error pointers
    cme_static_error_t *errors =
        malloc(sizeof(cme_static_error_t *) * current_batch);
    if (!errors) {
      perror("malloc failed");
      return 1;
    }

    // Create errors
    for (int j = 0; j < current_batch; ++j) {
      errors[j] = some_function(i + j);
    }

    // We need to touch values to elevate cpu hot cache, if you comment this
    //  out you will see how much touching defragmented memory costs.
    for (int j = 0; j < current_batch; ++j) {
      cme_static_error_t err = errors[j];
      size_t a = strlen(err->msg);
      size_t b = a + strlen(err->source_file);
      size_t c = b + strlen(err->source_func);
      a += c;
    }

#ifdef CME_ENABLE_BACKTRACE
    printf("---- DUMPING STACKTRACE FOR ERROR #%d ----\n", i);
    cme_static_error_dump(errors[0], "error_dump.txt");
#endif

    /* // Create errors */
    /* for (int j = 0; j < current_batch; ++j) { */
    /* } */

    // Destroy errors
    for (int j = 0; j < current_batch; ++j) {
      cme_static_error_destroy(errors[j]);
    }

    free(errors);
  }

  long long end_ns = cme_now_ns();
  double elapsed_ms = (end_ns - start_ns) / 1e6;

  printf("Static error allocation test complete:\n");
  printf("  Total errors: %d\n", max);
  printf("  Batch size: %d\n", batch);
  printf("  Bytes per batch: %zu bytes\n",
         sizeof(struct cme_StaticError) * batch);
  printf("  Time elapsed: %.4f ms\n", elapsed_ms);

  return 0;
}
