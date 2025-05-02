// bench.c
#include "c_minilib_error.h"
#include <stdio.h>
#include <time.h>

// --- 5-level error trace ---
static cme_error_t err_l1(void) { return cme_error(1, "Some error string"); }
static cme_error_t err_l2(void) { return cme_return(err_l1()); }
static cme_error_t err_l3(void) { return cme_return(err_l2()); }
static cme_error_t err_l4(void) { return cme_return(err_l3()); }
static cme_error_t err_l5(void) { return cme_return(err_l4()); }

// --- 5-level int return ---
static int int_l1(void) { return 1; }
static int int_l2(void) { return int_l1(); }
static int int_l3(void) { return int_l2(); }
static int int_l4(void) { return int_l3(); }
static int int_l5(void) { return int_l4(); }

int main(void) {
  const int iters = 1000000;
  struct timespec t0, t1;
  double ns_err, ns_int;

  // — Measure error trace —
  cme_init();
  clock_gettime(CLOCK_MONOTONIC, &t0);
  for (int i = 0; i < iters; ++i) {
    err_l5();
  }
  clock_gettime(CLOCK_MONOTONIC, &t1);
  cme_destroy();
  ns_err = (t1.tv_sec - t0.tv_sec) * 1e9 + (t1.tv_nsec - t0.tv_nsec);

  // — Measure plain int return —
  clock_gettime(CLOCK_MONOTONIC, &t0);
  for (int i = 0; i < iters; ++i) {
    int_l5();
  }
  clock_gettime(CLOCK_MONOTONIC, &t1);
  ns_int = (t1.tv_sec - t0.tv_sec) * 1e9 + (t1.tv_nsec - t0.tv_nsec);

  printf("5-level trace avg: %.1f ns\n", ns_err / iters);
  printf("5-level int   avg: %.1f ns\n", ns_int / iters);

  // — Generate and print one real error trace —
  cme_init();
  cme_error_t e = err_l5();
  if (e) {
    char buf[1024];
    if (cme_error_dump_to_str(e, sizeof(buf), buf) == 0) {
      printf("\n=== Sample 5-level Error Trace ===\n%s", buf);
    }
    cme_error_destroy(e);
  }
  cme_destroy();

  return 0;
}
