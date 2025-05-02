// test/test_c_minilib_error.d/test_c_minilib_error_with_backtrace.c
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "c_minilib_error.h"
#include "common.h"
#include <unity.h>

// Verifies code, message, and the first backtrace symbol fields
void test_create_error_with_backtrace_fields(void) {
  struct cme_Error *err = cme_errorf(300, "Field test");
  TEST_ASSERT_NOT_NULL(err);

  TEST_ASSERT_EQUAL(300, err->code);
  TEST_ASSERT_EQUAL_STRING("Field test", err->msg);
  TEST_ASSERT_EQUAL(1, err->stack_length);

  const struct cme_StackSymbol *sym = &err->stack_symbols[0];
  TEST_ASSERT_NOT_NULL(sym->source_file);

  TEST_ASSERT_TRUE(
      strstr(sym->source_file, "test_c_minilib_error_with_backtrace.c"));

  TEST_ASSERT_EQUAL_STRING("test_create_error_with_backtrace_fields",
                           sym->source_func);

  TEST_ASSERT_TRUE(sym->source_line > 0);

  cme_error_destroy(err);
}

void test_cme_error_dump_with_backtrace(void) {
  struct cme_Error err;
  err.code = 42;
  err.stack_length = 1;
  strncpy(err.msg, "Temporary error message", CME_STR_MAX);
  err.stack_symbols[0].source_file = "test_tmp.c";
  err.stack_symbols[0].source_func = "test_tmp_func";
  err.stack_symbols[0].source_line = 100;

  char *temp_file = create_temp_file_path();
  TEST_ASSERT_NOT_NULL(temp_file);

  int ret = cme_error_dump_to_file(&err, temp_file);
  TEST_ASSERT_EQUAL_INT(0, ret);

  FILE *fp = fopen(temp_file, "r");
  TEST_ASSERT_NOT_NULL(fp);

  char buf[2048];
  size_t n = fread(buf, 1, sizeof(buf) - 1, fp);
  buf[n] = '\0';
  fclose(fp);

  TEST_ASSERT_NOT_NULL(strstr(buf, "====== ERROR DUMP ======"));
  TEST_ASSERT_NOT_NULL(strstr(buf, "Error code: 42"));
  TEST_ASSERT_NOT_NULL(strstr(buf, err.msg));
  TEST_ASSERT_NOT_NULL(strstr(buf, "0:test_tmp_func:test_tmp.c:100"));

  remove(temp_file);
  free(temp_file);
}

// Helper wrappers for testing cme_return
static cme_error_t return_level1(void) {
  return cme_errorf(200, "Level1 error");
}

static cme_error_t return_level2(void) { return cme_return(return_level1()); }

void test_cme_return_macro(void) {
  cme_error_t err = return_level2();
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(200, err->code);

  // Expect two frames: level1 then level2
  TEST_ASSERT_EQUAL(2, err->stack_length);

  const struct cme_StackSymbol *sym0 = &err->stack_symbols[0];
  const struct cme_StackSymbol *sym1 = &err->stack_symbols[1];

  // First symbol should come from return_level1()
  TEST_ASSERT_TRUE(
      strstr(sym0->source_file, "test_c_minilib_error_with_backtrace.c"));
  TEST_ASSERT_EQUAL_STRING("return_level1", sym0->source_func);
  TEST_ASSERT_TRUE(sym0->source_line > 0);

  // Second symbol should come from return_level2()
  TEST_ASSERT_TRUE(
      strstr(sym1->source_file, "test_c_minilib_error_with_backtrace.c"));
  TEST_ASSERT_EQUAL_STRING("return_level2", sym1->source_func);
  TEST_ASSERT_TRUE(sym1->source_line > 0);

  cme_error_destroy(err);
}
