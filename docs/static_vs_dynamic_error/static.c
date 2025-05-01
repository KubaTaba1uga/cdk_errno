#include "static.h"

#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifdef CME_ENABLE_BACKTRACE
#include <dlfcn.h>
#include <execinfo.h>
#endif

#ifdef CME_ENABLE_BACKTRACE
static void resolve_address(void *addr, char *out, size_t out_size) {
  Dl_info info;
  if (dladdr(addr, &info) && info.dli_sname) {
    snprintf(out, out_size, "%s [%p]", info.dli_sname, addr);
  } else {
    snprintf(out, out_size, "?? [%p]", addr);
  }
}
#endif

cme_static_error_t cme_static_error_create(int code, const char *file,
                                           const char *func, int line,
                                           const char *fmt, ...) {
  cme_static_error_t err = malloc(sizeof(struct cme_StaticError));
  if (!err)
    return NULL;

  err->code = code;
  err->source_line = line;
  // file and func are string literals so we do not need to copy them
  err->source_file = file;
  err->source_func = func;

  if (fmt) {
    va_list args;
    va_start(args, fmt);
    vsnprintf(err->msg, CME_STR_MAX, fmt, args);
    va_end(args);
  } else {
    strncpy(err->msg, "No message", CME_STR_MAX - 1);
    err->msg[CME_STR_MAX - 1] = '\0';
  }

#ifdef CME_ENABLE_BACKTRACE
  void *frames[CME_STACK_MAX + 2];
  int total = backtrace(frames, CME_STACK_MAX + 2);

  if (total > 2) {
    int n = total - 2;
    if (n > CME_STACK_MAX)
      n = CME_STACK_MAX;

    memcpy(err->stack_addrs, frames + 2, n * sizeof(void *));
    err->stack_length = n;
  }

#else
  err->stack_length = 0;
#endif

  return err;
}

void cme_static_error_destroy(cme_static_error_t err) {
  free(err); // All contents are stack-like; only the struct is malloc'd
}

int cme_static_error_dump(cme_static_error_t err, const char *path) {
  if (!err || !path)
    return EINVAL;

  FILE *file = fopen(path, "w");
  if (!file)
    return errno;

  fprintf(file,
          "====== STATIC ERROR DUMP ======\n"
          "Error code: %d\n"
          "Error message: %s\n"
          "Src file: %s\n"
          "Src line: %d\n"
          "Src func: %s\n",
          err->code, err->msg, err->source_file, err->source_line,
          err->source_func);

#ifdef CME_ENABLE_BACKTRACE
  if (err->stack_length > 0) {
    fprintf(file, "------------------------\n");
    for (int i = 0; i < err->stack_length; ++i) {
      Dl_info info;
      const void *addr = err->stack_addrs[i];

      if (dladdr(addr, &info) && info.dli_sname) {
        fprintf(file, "%s+0x%lx [%p]\n", info.dli_sname,
                (unsigned long)((char *)addr - (char *)info.dli_saddr), addr);
      } else {
        fprintf(file, "?? [%p]\n", addr);
      }
    }
  }
#endif

  fclose(file);
  return 0;
}
