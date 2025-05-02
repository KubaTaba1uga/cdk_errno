#ifndef C_MINILIB_ERROR_TEST_COMMON_H
#define C_MINILIB_ERROR_TEST_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

inline static char *create_temp_file_path(void) {
  char *template = strdup("/tmp/cme_error_dump_XXXXXX");
  if (!template) {
    return NULL;
  }
  int fd = mkstemp(template);
  if (fd == -1) {
    free(template);
    return NULL;
  }
  close(fd);
  return template;
}

#endif // C_MINILIB_ERROR_TEST_COMMON_H
