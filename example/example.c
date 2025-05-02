#include <stdio.h>
#include <stdlib.h>

#include "c_minilib_error.h"

typedef struct cme_Error *cme_error_t;

cme_error_t error_function(void) {
  return cme_error(1, "Example error occurred with value: 42");
}

cme_error_t good_function(void) { return NULL; }

cme_error_t demo_error_logic(void) {
  struct cme_Error *err;

  if ((err = good_function())) {
    return cme_return(err);
  }

  if ((err = error_function())) {
    return cme_return(err);
  }

  return NULL;
}

void app() {
  cme_init();

  cme_error_t err;

  if ((err = demo_error_logic())) {
    const char *dump_file = "error_dump.txt";
    cme_error_dump_to_file(err, (char *)dump_file);
    char err_buffer[4096];
    cme_error_dump_to_str(err, sizeof(err_buffer), err_buffer);
    printf("%s", err_buffer);
    cme_error_destroy(err);
  }

  cme_destroy();
}

int main(void) {
  app();
  return 0;
}
