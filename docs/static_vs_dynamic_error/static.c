#define _POSIX_C_SOURCE 200809L
#include "static.h"
#include "common.h"

cme_static_error_t cme_static_error_create(int code, const char *file,
                                           const char *func, int line,
                                           const char *fmt, ...) {
  cme_static_error_t err = malloc(sizeof(struct cme_StaticError));
  if (!err)
    return NULL;

  err->code = code;
  err->source_line = line;
  err->source_file = file;
  err->source_func = func;

  if (fmt) {
    va_list args;
    va_start(args, fmt);
    cme_format_message_va(err->msg, CME_STR_MAX, fmt, args);
    va_end(args);
  } else {
    strncpy(err->msg, "No message", CME_STR_MAX - 1);
    err->msg[CME_STR_MAX - 1] = '\0';
  }

#ifdef CME_ENABLE_BACKTRACE
  err->stack_length =
      cme_capture_backtrace(err->stack_addrs, CME_STACK_MAX, NULL);
#else
  err->stack_length = 0;
#endif

  return err;
}

void cme_static_error_destroy(cme_static_error_t err) {
  free(err); // Only the struct is malloc’d
}

int cme_static_error_dump(cme_static_error_t err, const char *path) {
  if (!err || !path)
    return EINVAL;

  FILE *file = fopen(path, "w");
  if (!file)
    return errno;

  CME_DUMP_COMMON_FIELDS(file, err);

#ifdef CME_ENABLE_BACKTRACE
  cme_dump_backtrace(file, err->stack_length, err->stack_addrs);
#endif

  fclose(file);
  return 0;
}
