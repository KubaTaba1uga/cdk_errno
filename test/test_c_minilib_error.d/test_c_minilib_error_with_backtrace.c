#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <unity.h>

#include "c_minilib_error.h"
#include "common.h"

void test_create_error_with_backtrace(void) {
  struct cme_Error *err = cme_errorf(101, "Backtrace test");

  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(101, err->code);
  TEST_ASSERT_NOT_NULL(err->msg);
  TEST_ASSERT_TRUE(err->stack_length == 0);

  cme_error_destroy(err);
}

void test_cme_error_dump_with_backtrace(void) {
  struct cme_Error err = {
      .code = 42,
      .source_line = 100,
      .stack_length = 1,
  };
  strncpy(err.msg, "Temporary error message", CME_STR_MAX);
  strncpy(err.source_file, "test_tmp.c", CME_STR_MAX);
  strncpy(err.source_func, "test_tmp_func", CME_STR_MAX);

  void *dummy_symbols[] = {(void *)0x1234abcd};
  memcpy(err.stack_symbols, dummy_symbols, sizeof(dummy_symbols));

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
  TEST_ASSERT_NOT_NULL(strstr(buf, err.source_file));
  TEST_ASSERT_NOT_NULL(strstr(buf, "Src line: 100"));
  TEST_ASSERT_NOT_NULL(strstr(buf, err.source_func));
  TEST_ASSERT_NOT_NULL(strstr(buf, "0x"));

  remove(temp_file);
  free(temp_file);
}
