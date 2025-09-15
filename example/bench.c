#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "cdk_errno.h"

#define NOINLINE __attribute__((noinline))

// — original 5-level error trace —
static NOINLINE cdk_error_t err_l1(void) { return cdk_error(1, "Some error"); }
static NOINLINE cdk_error_t err_l2(void) { return cdk_ewrap(err_l1()); }
static NOINLINE cdk_error_t err_l3(void) { return cdk_ewrap(err_l2()); }
static NOINLINE cdk_error_t err_l4(void) { return cdk_ewrap(err_l3()); }
static NOINLINE cdk_error_t err_l5(void) { return cdk_ewrap(err_l4()); }

// — 5-level formatted-error trace —
static NOINLINE cdk_error_t errf_l1(void) {
  return cdk_errorf(1, "Error #%d occurred", 1);
}
static NOINLINE cdk_error_t errf_l2(void) { return cdk_ewrap(errf_l1()); }
static NOINLINE cdk_error_t errf_l3(void) { return cdk_ewrap(errf_l2()); }
static NOINLINE cdk_error_t errf_l4(void) { return cdk_ewrap(errf_l3()); }
static NOINLINE cdk_error_t errf_l5(void) { return cdk_ewrap(errf_l4()); }

// — 5-level int return —
static volatile int __i__ = 0;
static NOINLINE int int_l1(void) { return __i__++; }
static NOINLINE int int_l2(void) { return int_l1(); }
static NOINLINE int int_l3(void) { return int_l2(); }
static NOINLINE int int_l4(void) { return int_l3(); }
static NOINLINE int int_l5(void) { return int_l4(); }

int main(void) {
  const int iters = 1000000;
  struct timespec t0, t1;
  double ns_err, ns_fmt, ns_int;
  volatile int sink = 0;
  volatile cdk_error_t err;

  // measure unformatted error-trace
  clock_gettime(CLOCK_MONOTONIC, &t0);
  for (int i = 0; i < iters; i++) {
    err = err_l5();
  }
  clock_gettime(CLOCK_MONOTONIC, &t1);
  ns_err = (t1.tv_sec - t0.tv_sec) * 1e9 + (t1.tv_nsec - t0.tv_nsec);

  // measure formatted error-trace
  clock_gettime(CLOCK_MONOTONIC, &t0);
  for (int i = 0; i < iters; i++) {
    err = errf_l5();
  }
  clock_gettime(CLOCK_MONOTONIC, &t1);
  ns_fmt = (t1.tv_sec - t0.tv_sec) * 1e9 + (t1.tv_nsec - t0.tv_nsec);

  // measure plain int return
  clock_gettime(CLOCK_MONOTONIC, &t0);
  for (int i = 0; i < iters; i++) {
    sink = int_l5();
  }
  clock_gettime(CLOCK_MONOTONIC, &t1);
  ns_int = (t1.tv_sec - t0.tv_sec) * 1e9 + (t1.tv_nsec - t0.tv_nsec);

  printf("5-lvl trace avg:     %.1f ns\n", ns_err / iters);
  printf("5-lvl fmt-trace avg: %.1f ns\n", ns_fmt / iters);
  printf("5-lvl int   avg:     %.1f ns\n", ns_int / iters);

  // sample dump
  cdk_error_t e = errf_l5();
  if (e) {
    char buf[1024];
    if (cdk_error_dump_to_str(e, sizeof(buf), buf) == 0) {
      printf("\n=== Sample Formatted Error Trace ===\n%s", buf);
    } else {
      printf("Failed to dump error\n");
    }
  }

  // sample dump
  e = err_l5();
  if (e) {
    char buf[1024];
    if (cdk_error_dump_to_str(e, sizeof(buf), buf) == 0) {
      printf("\n=== Sample Error Trace ===\n%s", buf);
    } else {
      printf("Failed to dump error\n");
    }
  }

  (void)sink;
  (void)err;

  return 0;
}
