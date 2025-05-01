#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "method_backtrace.h"
#include "method_manual.h"

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
  // === POSIX BACKTRACE ===
  void **trace_posix = NULL;
  int frames_posix = 0;
  double total_ns_posix = 0.0;

  double start = get_time_ns();
  for (int i = 0; i < N_ITER; ++i) {
    void **buffer = malloc(sizeof(void *) * TRACE_DEPTH);
    if (!buffer) {
      perror("malloc");
      return 1;
    }

    run_backtrace_posix(buffer, TRACE_DEPTH, &frames_posix);

    if (i == N_ITER - 1) {
      trace_posix = buffer; // save last
    } else {
      free(buffer);
    }
  }
  double end = get_time_ns();
  total_ns_posix = end - start;

  double total_ms_posix = total_ns_posix / 1e6;
  double avg_ns_posix = total_ns_posix / N_ITER;

  printf("POSIX backtrace: total time = %.2f ms, avg time = %.2f ns over %d "
         "iterations\n",
         total_ms_posix, avg_ns_posix, N_ITER);

  printf("Example POSIX backtrace (raw addresses):\n");
  cme_dump_backtrace(stdout, frames_posix, trace_posix);
  free(trace_posix);

  // === MANUAL BUILTIN RETURN ADDRESS TRACE ===
  void **trace_manual = NULL;
  int frames_manual = 0;
  double total_ns_manual = 0.0;

  start = get_time_ns();
  for (int i = 0; i < N_ITER; ++i) {
    void **buffer = malloc(sizeof(void *) * TRACE_DEPTH);
    if (!buffer) {
      perror("malloc");
      return 1;
    }

    run_manual_trace(buffer, TRACE_DEPTH, &frames_manual);

    if (i == N_ITER - 1) {
      trace_manual = buffer; // save last
    } else {
      free(buffer);
    }
  }
  end = get_time_ns();
  total_ns_manual = end - start;

  double total_ms_manual = total_ns_manual / 1e6;
  double avg_ns_manual = total_ns_manual / N_ITER;

  printf("Manual return address trace: total time = %.2f ms, avg time = %.2f "
         "ns over %d iterations\n",
         total_ms_manual, avg_ns_manual, N_ITER);

  printf("Example manual trace (raw addresses):\n");
  cme_dump_backtrace(stdout, frames_manual, trace_manual);
  free(trace_manual);

  return 0;
}
