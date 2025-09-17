#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <time.h>

#include "example_2_lib.h" // errno-style wrapper (TLS defined in example_2_lib.c)

#define NOINLINE __attribute__((noinline))

// — 5-level error trace (literal string) —
static NOINLINE int err_l1(void) {
  cdk_errno = cdk_errnos(1, "Some error");
  return -1;
}
static NOINLINE int err_l2(void) {
  int r = err_l1();
  if (r < 0) {
    return cdk_ereturn(-1);
  }
  return r;
}
static NOINLINE int err_l3(void) {
  int r = err_l2();
  if (r < 0) {
    return cdk_ereturn(-1);
  }
  return r;
}
static NOINLINE int err_l4(void) {
  int r = err_l3();
  if (r < 0) {
    return cdk_ereturn(-1);
  }
  return r;
}
static NOINLINE int err_l5(void) {
  int r = err_l4();
  if (r < 0) {
    return cdk_ereturn(-1);
  }
  return r;
}

// — 5-level formatted-error trace —
#ifndef CDK_ERROR_OPTIMIZE
static NOINLINE int errf_l1(void) {
  cdk_errno = cdk_errnof(1, "Error #%d occurred", 1);
  return -1;
}
static NOINLINE int errf_l2(void) {
  int r = errf_l1();
  if (r < 0) {
    return cdk_ereturn(-1);
  }
  return r;
}
static NOINLINE int errf_l3(void) {
  int r = errf_l2();
  if (r < 0) {
    return cdk_ereturn(-1);
  }
  return r;
}
static NOINLINE int errf_l4(void) {
  int r = errf_l3();
  if (r < 0) {
    return cdk_ereturn(-1);
  }
  return r;
}
static NOINLINE int errf_l5(void) {
  int r = errf_l4();
  if (r < 0) {
    return cdk_ereturn(-1);
  }
  return r;
}
#endif

// — 5-level plain int return —
static volatile int __i__ = 0;
static NOINLINE int int_l1(void) { return __i__++; }
static NOINLINE int int_l2(void) { return int_l1(); }
static NOINLINE int int_l3(void) { return int_l2(); }
static NOINLINE int int_l4(void) { return int_l3(); }
static NOINLINE int int_l5(void) { return int_l4(); }

static inline double ns_since(const struct timespec *a,
                              const struct timespec *b) {
  return (b->tv_sec - a->tv_sec) * 1e9 + (b->tv_nsec - a->tv_nsec);
}

int main(void) {
  const int iters = 1000000;
  struct timespec t0, t1;
  double ns_err = 0.0, ns_fmt = 0.0, ns_int = 0.0;
  volatile int sink = 0;

  // measure unformatted errno-trace
  cdk_errno = 0;
  clock_gettime(CLOCK_MONOTONIC, &t0);
  for (int i = 0; i < iters; i++) {
    sink ^= err_l5();
    cdk_errno = 0; // clear between iterations to avoid growing traces
  }
  clock_gettime(CLOCK_MONOTONIC, &t1);
  ns_err = ns_since(&t0, &t1);

#ifndef CDK_ERROR_OPTIMIZE
  // measure formatted errno-trace
  cdk_errno = 0;
  clock_gettime(CLOCK_MONOTONIC, &t0);
  for (int i = 0; i < iters; i++) {
    sink ^= errf_l5();
    cdk_errno = 0;
  }
  clock_gettime(CLOCK_MONOTONIC, &t1);
  ns_fmt = ns_since(&t0, &t1);
#endif

  // measure plain int return
  clock_gettime(CLOCK_MONOTONIC, &t0);
  for (int i = 0; i < iters; i++) {
    sink ^= int_l5();
  }
  clock_gettime(CLOCK_MONOTONIC, &t1);
  ns_int = ns_since(&t0, &t1);

  printf("5-lvl errno-trace avg:     %.1f ns\n", ns_err / iters);
#ifndef CDK_ERROR_OPTIMIZE
  printf("5-lvl fmt errno-trace avg: %.1f ns\n", ns_fmt / iters);
#else
  printf("5-lvl fmt errno-trace avg: (disabled by CDK_ERROR_OPTIMIZE)\n");
#endif
  printf("5-lvl int           avg:   %.1f ns\n", ns_int / iters);

  (void)sink; // keep side effects
  (void)ns_fmt;

  return 0;
}
