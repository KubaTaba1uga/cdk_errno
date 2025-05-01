/* dynamic.c */
#define _POSIX_C_SOURCE 200809L
#include "dynamic.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef CME_ENABLE_BACKTRACE
#include <dlfcn.h>
#include <execinfo.h>
#endif

static struct cme_DynamicError generic_error = {
    .code = -1,
    .msg = "Generic dynamic error",
    .source_file = __FILE__,
    .source_func = NULL,
    .source_line = 0,
    .stack_length = 0,
    .stack_addrs = NULL,
};

#define CREATE_GENERIC_ERROR(code_, msg_)                                      \
  do {                                                                         \
    generic_error.code = (code_);                                              \
    generic_error.msg = (msg_);                                                \
    generic_error.source_func = __func__;                                      \
    generic_error.source_line = __LINE__;                                      \
  } while (0)

cme_dynamic_error_t cme_dynamic_error_create(int code, const char *file,
                                             const char *func, int line,
                                             const char *fmt, ...) {
  cme_dynamic_error_t err = malloc(sizeof(*err));
  if (!err) {
    CREATE_GENERIC_ERROR(ENOMEM, "Alloc struct failed");
    return &generic_error;
  }

  err->code = code;
  err->source_line = line;
  // file and func are string literals so we do not need to copy them
  err->source_file = file;
  err->source_func = func;

  /* format the message */
  if (fmt) {
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);

    if (len < 0) {
      cme_dynamic_error_destroy(err);
      CREATE_GENERIC_ERROR(EINVAL, "Bad fmt args");
      return &generic_error;
    }

    err->msg = malloc(len + 1);
    if (!err->msg) {
      cme_dynamic_error_destroy(err);
      CREATE_GENERIC_ERROR(ENOMEM, "Alloc msg failed");
      return &generic_error;
    }

    va_start(ap, fmt);
    vsnprintf(err->msg, len + 1, fmt, ap);
    va_end(ap);
  } else {
    err->msg = strdup("No message");
  }

#ifdef CME_ENABLE_BACKTRACE
  /* capture raw addresses, then heap‐allocate array */
  void *frames[CME_STACK_MAX + 2];
  int total = backtrace(frames, CME_STACK_MAX + 2) - 2;
  if (total > 0) {
    if (total > CME_STACK_MAX)
      total = CME_STACK_MAX;
    err->stack_length = total;
    err->stack_addrs = malloc(sizeof(void *) * total);
    if (err->stack_addrs)
      memcpy(err->stack_addrs, frames + 2, total * sizeof(void *));
  }
#else
  err->stack_length = 0;
#endif

  return err;
}

void cme_dynamic_error_destroy(cme_dynamic_error_t err) {
  if (!err || err == &generic_error)
    return;
  free(err->msg);
#ifdef CME_ENABLE_BACKTRACE
  free(err->stack_addrs);
#endif
  free(err);
}

int cme_dynamic_error_dump(cme_dynamic_error_t err, const char *path) {
  if (!err || !path)
    return EINVAL;
  FILE *f = fopen(path, "w");
  if (!f)
    return errno;

  fprintf(f,
          "====== DYNAMIC ERROR DUMP ======\n"
          "Error code: %d\n"
          "Error string: %s\n"
          "Error message: %s\n"
          "Src file: %s\n"
          "Src line: %d\n"
          "Src func: %s\n",
          err->code, strerror(err->code), err->msg ? err->msg : "NULL",
          err->source_file, err->source_line,
          err->source_func ? err->source_func : "NULL");

#ifdef CME_ENABLE_BACKTRACE
  if (err->stack_length > 0 && err->stack_addrs) {
    fprintf(f, "------------------------\n");
    for (int i = 0; i < err->stack_length; ++i) {
      Dl_info info;
      void *addr = err->stack_addrs[i];
      if (dladdr(addr, &info) && info.dli_sname) {
        unsigned long off = (char *)addr - (char *)info.dli_saddr;
        fprintf(f, "%s+0x%lx [%p]\n", info.dli_sname, off, addr);
      } else {
        fprintf(f, "?? [%p]\n", addr);
      }
    }
  }
#endif

  fclose(f);
  return 0;
}
