#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "method_asm.h" // ← include the new stack-RA method
#include "method_backtrace.h"
#include "method_manual.h"

#define N_ITER 100000
#define TRACE_DEPTH 16

// Dump backtrace to file
static inline void cme_dump_backtrace(FILE *f, int count, void **addrs) {
  if (!f || count <= 0 || !addrs)
    return;
  fprintf(f, "------------------------\n");
  for (int i = 0; i < count; ++i)
    fprintf(f, "[%p]\n", addrs[i]);
}

static inline double get_time_ns() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (double)ts.tv_sec * 1e9 + ts.tv_nsec;
}

int main(void) {
  // === POSIX backtrace() ===
  void **trace_posix = NULL;
  int frames_posix = 0;
  double total_ns_posix = 0.0;

  double start = get_time_ns();
  for (int i = 0; i < N_ITER; ++i) {
    void **buf = malloc(sizeof(void *) * TRACE_DEPTH);
    if (!buf) {
      perror("malloc");
      return 1;
    }
    run_backtrace_posix(buf, TRACE_DEPTH, &frames_posix);
    if (i == N_ITER - 1)
      trace_posix = buf;
    else
      free(buf);
  }
  double end = get_time_ns();
  total_ns_posix = end - start;

  printf("POSIX backtrace: total = %.2f ms, avg = %.2f ns/iter\n",
         total_ns_posix / 1e6, total_ns_posix / N_ITER);
  printf("Example POSIX raw addresses:\n");
  cme_dump_backtrace(stdout, frames_posix, trace_posix);
  free(trace_posix);

  // === manual __builtin_return_address() ===
  void **trace_manual = NULL;
  int frames_manual = 0;
  double total_ns_manual = 0.0;

  start = get_time_ns();
  for (int i = 0; i < N_ITER; ++i) {
    void **buf = malloc(sizeof(void *) * TRACE_DEPTH);
    if (!buf) {
      perror("malloc");
      return 1;
    }
    run_manual_trace(buf, TRACE_DEPTH, &frames_manual);
    if (i == N_ITER - 1)
      trace_manual = buf;
    else
      free(buf);
  }
  end = get_time_ns();
  total_ns_manual = end - start;

  printf("Manual RA trace: total = %.2f ms, avg = %.2f ns/iter\n",
         total_ns_manual / 1e6, total_ns_manual / N_ITER);
  printf("Example manual raw addresses:\n");
  cme_dump_backtrace(stdout, frames_manual, trace_manual);
  free(trace_manual);

  // === stack-based return-address capture (ASM) ===
  void **trace_asm = NULL;
  int frames_asm = 0;
  double total_ns_asm = 0.0;

  start = get_time_ns();
  for (int i = 0; i < N_ITER; ++i) {
    void **buf = malloc(sizeof(void *) * TRACE_DEPTH);
    if (!buf) {
      perror("malloc");
      return 1;
    }
    run_asm_trace(buf, TRACE_DEPTH, &frames_asm);
    if (i == N_ITER - 1)
      trace_asm = buf;
    else
      free(buf);
  }
  end = get_time_ns();
  total_ns_asm = end - start;

  printf("ASM stack-RA trace: total = %.2f ms, avg = %.2f ns/iter\n",
         total_ns_asm / 1e6, total_ns_asm / N_ITER);
  printf("Example ASM raw addresses:\n");
  cme_dump_backtrace(stdout, frames_asm, trace_asm);
  free(trace_asm);

  return 0;
}
