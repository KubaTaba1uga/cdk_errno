#include <string.h>

#include <unity.h>

#include "c_minilib_error.h"

void test_create_error_with_backtrace(void) {
  struct cme_Error *err = cme_errorf(101, "Backtrace test");

  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(101, err->code);
  TEST_ASSERT_NOT_NULL(err->msg);
  TEST_ASSERT_TRUE(err->stack_size > 0);
  TEST_ASSERT_NOT_NULL(err->stack_symbols);

  // Optional: Print a few frames for manual verification
  for (int i = 0; i < err->stack_size && i < 3; ++i) {
    printf("[backtrace #%d] %s\n", i, err->stack_symbols[i]);
  }

  cme_error_destroy(err);
}
