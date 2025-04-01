#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"

void demo_error_logic(void) {
  struct cme_Error *err =
      cme_errorf(1, "Example error occurred with value: %d", 42);

  const char *dump_file = "error_dump.txt";
  int dump_ret = cme_error_dump(err, (char *)dump_file);
  if (dump_ret == 0) {
    printf("Error dump written to '%s'\n", dump_file);
  } else {
    printf("Failed to dump error. Error code: %d\n", dump_ret);
  }

  cme_error_destroy(err);
}

void func1() { demo_error_logic(); }

void func2() { func1(); }

int main(void) {
  func2();
  return 0;
}
