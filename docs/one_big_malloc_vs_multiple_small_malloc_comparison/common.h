#ifndef C_MINILIB_COMMON_H
#define C_MINILIB_COMMON_H

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <time.h>

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

#define GET_CURRENT_STACK_ADDR()                                               \
  __builtin_extract_return_addr(__builtin_return_address(0));

struct cme_StackSymbol {
  const char *source_file;
  const char *source_func;
  int source_line;
};

// Timestamp helper
static inline long long cme_now_ns(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

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

static inline void cme_dump_backtrace(FILE *f, int count,
                                      const struct cme_StackSymbol *stack) {
  if (!f || count <= 0 || !stack)
    return;

  fprintf(f, "------------------------\n");
  for (int i = 0; i < count; ++i) {
    fprintf(f, "#%d: %s:%d in %s()\n", i,
            stack[i].source_file ? stack[i].source_file : "???",
            stack[i].source_line,
            stack[i].source_func ? stack[i].source_func : "???");
  }
}
#endif
