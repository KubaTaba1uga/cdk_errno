/* Build via: gcc -O1 -g -fno-omit-frame-pointer -rdynamic test_backtrace.c -ldl
 * -o test_backtrace */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <execinfo.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// Resolve symbol name from address
__attribute__((noinline)) void resolve(void *addr) {
  Dl_info info;
  if (dladdr(addr, &info) && info.dli_sname)
    printf("Symbol: %s (addr: %p)\n", info.dli_sname, addr);
  else
    printf("Unknown address: %p\n", addr);
}

// Fast frame-pointer capture (x86-64 only)
__attribute__((noinline)) void capture_two_fast(void **out) {
  void *rbp;
  __asm__ volatile("mov %%rbp, %0" : "=r"(rbp));
  out[0] = __builtin_return_address(0);

  if (rbp) {
    for (int i = 0; i < 2; i++) {
      void *next = *((void **)rbp);
      if (next)
        out[i] = *((void **)next + 1);
      else
        out[i] = NULL;
      rbp = next;
    }
  } else {
    out[1] = NULL;
  }
}

// Classical GLIBC backtrace-based capture
__attribute__((noinline)) int capture_two_backtrace(void **out) {
  void *frames[16];
  int total = backtrace(frames, 16);
  if (total <= 2)
    return 0;
  int count = (total - 2 < 2) ? (total - 2) : 2;
  memcpy(out, frames + 2, count * sizeof(void *));
  return count;
}

// Compare both methods
__attribute__((noinline)) void print_trace() {
  void *addrs_fast[2];
  void *addrs_bt[2];
  struct timespec start, end;

  // Time fast capture
  clock_gettime(CLOCK_MONOTONIC, &start);
  capture_two_fast(addrs_fast);
  clock_gettime(CLOCK_MONOTONIC, &end);
  long long delta_fast = (end.tv_sec - start.tv_sec) * 1000000000LL +
                         (end.tv_nsec - start.tv_nsec);

  // Time classical backtrace
  clock_gettime(CLOCK_MONOTONIC, &start);
  int n = capture_two_backtrace(addrs_bt);
  clock_gettime(CLOCK_MONOTONIC, &end);
  long long delta_bt = (end.tv_sec - start.tv_sec) * 1000000000LL +
                       (end.tv_nsec - start.tv_nsec);

  // Show both results
  printf("\n[Fast capture via frame pointer]\n");
  for (int i = 0; i < 2; ++i) {
    printf("Return address %d: %p -> ", i, addrs_fast[i]);
    resolve(addrs_fast[i]);
  }
  printf("capture_two_fast() took %lld ns\n", delta_fast);

  printf("\n[Classical backtrace() capture]\n");
  for (int i = 0; i < n; ++i) {
    printf("Backtrace addr %d: %p -> ", i, addrs_bt[i]);
    resolve(addrs_bt[i]);
  }
  printf("capture_two_backtrace() took %lld ns\n", delta_bt);
}

// Test call chain
__attribute__((noinline)) void func2() { print_trace(); }
__attribute__((noinline)) void func1() { func2(); }

int main() {
  func1();
  return 0;
}
