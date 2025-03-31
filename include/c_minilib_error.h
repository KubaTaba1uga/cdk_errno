#ifndef C_MINILIB_ERROR_H
#define C_MINILIB_ERROR_H

struct cme_Error {
  int code;
  char *msg;
  char *source_file;
  char *source_func;
  int source_line;
};

struct cme_Error *cme_error_create(int code, char *source_file,
                                   char *source_func, int source_line,
                                   char *fmt, ...);

void cme_error_destroy(struct cme_Error *);

#define CME_ERRORF(code, fmt, ...)                                             \
  cme_error_create((code), (char *)__FILE__, (char *)__func__, __LINE__,       \
                   (char *)(fmt), ##__VA_ARGS__)

#endif // C_MINILIB_CONFIG_ERROR_H
