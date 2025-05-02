#define _POSIX_C_SOURCE 200809L
#include "c_minilib_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

extern cme_error_t alloc_malloc_errorf(int, const char *, ...);
extern void alloc_malloc_free(cme_error_t);

extern void alloc_ringbuf_init(void);
extern void alloc_ringbuf_cleanup(void);
extern cme_error_t alloc_ringbuf_errorf(int, const char *, ...);

#define N_ITER 1000000

static inline double get_time_ns(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1e9 + ts.tv_nsec;
}

int cme_error_dump_to_str(cme_error_t err, uint32_t size, char *buf) {
  return snprintf(buf, size, "Error: %d, Msg: %s\n", err->code, err->msg);
}

void cme_error_destroy(cme_error_t err) {
  free(err); // only used for malloc-allocated
}

void run_malloc_benchmark(void) {
  cme_error_t last_err = NULL;

  double start = get_time_ns();
  for (int i = 0; i < N_ITER; ++i) {
    cme_error_t err = alloc_malloc_errorf(42, "Malloc error %d", i);
    if (i == N_ITER - 1)
      last_err = err;
    else
      alloc_malloc_free(err);
  }
  double end = get_time_ns();

  printf("Malloc strategy:         total = %.2f ms, avg = %.2f ns/iter\n",
         (end - start) / 1e6, (end - start) / N_ITER);

  // Dump final sample outside benchmark loop
  char buf[512];
  cme_error_dump_to_str(last_err, sizeof(buf), buf);
  printf("Example malloc error:\n%s", buf);
  alloc_malloc_free(last_err);
}

void run_ringbuf_benchmark(void) {
  alloc_ringbuf_init();
  cme_error_t last_err = NULL;

  double start = get_time_ns();
  for (int i = 0; i < N_ITER; ++i) {
    cme_error_t err = alloc_ringbuf_errorf(43, "Ring error %d", i);
    if (i == N_ITER - 1)
      last_err = err;
  }
  double end = get_time_ns();

  printf("Ring buffer strategy:    total = %.2f ms, avg = %.2f ns/iter\n",
         (end - start) / 1e6, (end - start) / N_ITER);

  char buf[512];
  cme_error_dump_to_str(last_err, sizeof(buf), buf);
  printf("Example ringbuf error:\n%s", buf);

  alloc_ringbuf_cleanup();
}

int main(void) {
  run_malloc_benchmark();
  run_ringbuf_benchmark();
  return 0;
}
