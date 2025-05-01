#ifndef C_MINILIB_COMMON_H
#define C_MINILIB_COMMON_H

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <time.h>

#ifdef CME_ENABLE_BACKTRACE
#include <dlfcn.h>
#include <execinfo.h>
#endif

#define CME_DUMP_COMMON_FIELDS(f, e)                                           \
  do {                                                                         \
    if ((f) && (e)) {                                                          \
      fprintf((f),                                                             \
              "====== ERROR DUMP ======\n"                                     \
              "Error code: %d\n"                                               \
              "Error message: %s\n"                                            \
              "Src file: %s\n"                                                 \
              "Src line: %d\n"                                                 \
              "Src func: %s\n",                                                \
              (e)->code, (e)->msg ? (e)->msg : "NULL",                         \
              (e)->source_file ? (e)->source_file : "NULL", (e)->source_line,  \
              (e)->source_func ? (e)->source_func : "NULL");                   \
    }                                                                          \
  } while (0)

// Timestamp helper
static inline long long cme_now_ns(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

static inline long cme_now_mem_kb(void) {
  struct rusage r;
  if (getrusage(RUSAGE_SELF, &r) != 0) {
    return -1; /* error */
  }
  /* On Linux, ru_maxrss is already in KB */
  return r.ru_maxrss;
}

// Format message into preallocated buffer (va_list variant)
static inline int cme_format_message_va(char *buf, size_t buflen,
                                        const char *fmt, va_list ap) {
  if (!buf || buflen == 0)
    return EINVAL;

  if (!fmt) {
    strncpy(buf, "No message", buflen - 1);
    buf[buflen - 1] = '\0';
    return 0;
  }

  vsnprintf(buf, buflen, fmt, ap);
  return 0;
}

#ifdef CME_ENABLE_BACKTRACE
// Fill backtrace into preallocated array
static inline int cme_capture_backtrace(void **out, int max) {
  void *frames[max + 2];
  int total = backtrace(frames, max + 2);
  if (total <= 2) {
    return 0;
  }

  int n = total - 2;
  if (n > max)
    n = max;

  memcpy(out, frames + 2, n * sizeof(void *));

  return n;
}

// Dump backtrace to file
static inline void cme_dump_backtrace(FILE *f, int count, void **addrs) {
  if (!f || count <= 0 || !addrs)
    return;

  fprintf(f, "------------------------\n");
  for (int i = 0; i < count; ++i) {
    fprintf(f, "[%p]\n", addrs[i]);
  }
}
#endif

#endif // C_MINILIB_COMMON_H
