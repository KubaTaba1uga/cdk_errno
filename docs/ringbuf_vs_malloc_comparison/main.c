#include "c_minilib_error.h"
#include <stdio.h>
#include <stdlib.h>

extern cme_error_t cme_errorf_static(int code, const char *fmt, ...);

int main(void) {
  for (int i = 0; i < 10; ++i) {
    cme_error_t err = cme_errorf_static(100 + i, "Static error #%d", i);
    char buf[512];
    cme_error_dump_to_str(err, sizeof(buf), buf);
    puts(buf);
  }
  return 0;
}
