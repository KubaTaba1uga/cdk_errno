#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "method_backtrace.h"

#define N_ITER 100000
#define TRACE_DEPTH 16

// Dump backtrace to file
static inline void cme_dump_backtrace(FILE *f, int count, void **addrs) {
  if (!f || count <= 0 || !addrs)
    return;

  fprintf(f, "------------------------\n");
  for (int i = 0; i < count; ++i) {
    fprintf(f, "[%p]\n", addrs[i]);
  }
}

static inline double get_time_ns() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (double)ts.tv_sec * 1e9 + ts.tv_nsec;
}

int main(void) {
  void *trace_posix[TRACE_DEPTH];
  int frames_posix = 0;
  double total_ns_posix = 0.0;

  double start = get_time_ns();
  for (int i = 0; i < N_ITER; ++i) {
    run_backtrace_posix(trace_posix, TRACE_DEPTH, &frames_posix);
  }
  double end = get_time_ns();
  total_ns_posix = end - start;

  printf("POSIX backtrace: avg time = %.2f ns over %d iterations\n",
         total_ns_posix / N_ITER, N_ITER);

  // Print last backtrace using address dump
  printf("Example POSIX backtrace (raw addresses):\n");
  cme_dump_backtrace(stdout, frames_posix, trace_posix);

  return 0;
}
