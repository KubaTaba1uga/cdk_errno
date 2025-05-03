#include "c_minilib_error.h"
#include "common.h"
#include <string.h>
#include <unity.h>

void setUp() { cme_init(); }
void tearDown() { cme_destroy(); }

static cme_error_t bt_level1(void) {
  return cme_errorf(200, "Backtrace level 1");
}

static cme_error_t bt_level2(void) { return cme_return(bt_level1()); }

void test_backtrace_depth(void) {
  cme_error_t err = bt_level2();
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(200, err->code);
  TEST_ASSERT_EQUAL(2, err->frames_length);
}

void test_backtrace_symbol_fields(void) {
  cme_error_t err = bt_level2();
  TEST_ASSERT_NOT_NULL(err);

  const struct cme_Frame *f0 = &err->frames[0];
  const struct cme_Frame *f1 = &err->frames[1];

  TEST_ASSERT_NOT_NULL(f0->file);
  TEST_ASSERT_NOT_NULL(f0->func);
  TEST_ASSERT_TRUE(f0->line > 0);

  TEST_ASSERT_NOT_NULL(f1->file);
  TEST_ASSERT_NOT_NULL(f1->func);
  TEST_ASSERT_TRUE(f1->line > 0);
}

static cme_error_t return_level1(void) { return cme_error(100, "L1 error"); }
static cme_error_t return_level2(void) { return cme_return(return_level1()); }

void test_cme_return_macro_appends_frame(void) {
  cme_error_t err = return_level2();
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(100, err->code);
  TEST_ASSERT_EQUAL(2, err->frames_length);
}
