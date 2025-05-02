// test/test_c_minilib_error.d/test_c_minilib_error.c
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "c_minilib_error.h"
#include "common.h"
#include <unity.h>

void setUp() { cme_init(); }

void tearDown() { cme_destroy(); }

void test_create_error_success(void) {
  struct cme_Error *err = cme_error_create(42, "test.c", "test_func", 123,
                                           "Failed with code %d", 42);

  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(42, err->code);
  TEST_ASSERT_EQUAL_STRING("test.c", err->stack_symbols[0].source_file);
  TEST_ASSERT_EQUAL_STRING("test_func", err->stack_symbols[0].source_func);
  TEST_ASSERT_EQUAL(123, err->stack_symbols[0].source_line);
  TEST_ASSERT_EQUAL_STRING("Failed with code 42", err->msg);

  cme_error_destroy(err);
}

void test_create_error_null_source(void) {
  struct cme_Error *err = cme_error_create(99, NULL, NULL, 0, "Just a message");

  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(99, err->code);
  TEST_ASSERT_EQUAL_STRING("Just a message", err->msg);

  cme_error_destroy(err);
}

void test_create_error_invalid_format(void) {
  struct cme_Error *err = cme_error_create(1, "x.c", "func", 1, NULL);

  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(1, err->code);
  TEST_ASSERT_EQUAL_STRING("No message", err->msg);

  cme_error_destroy(err);
}

void test_destroy_null(void) { cme_error_destroy(NULL); }

void test_create_error_multiple_format_args(void) {
  struct cme_Error *err =
      cme_error_create(7, "multi.c", "multi_func", 77, "Error code %d at %s:%d",
                       7, "multi.c", 77);

  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(7, err->code);
  TEST_ASSERT_EQUAL_STRING("multi.c", err->stack_symbols[0].source_file);
  TEST_ASSERT_EQUAL_STRING("multi_func", err->stack_symbols[0].source_func);
  TEST_ASSERT_EQUAL(77, err->stack_symbols[0].source_line);
  TEST_ASSERT_EQUAL_STRING("Error code 7 at multi.c:77", err->msg);

  cme_error_destroy(err);
}

void test_create_error_no_format_string(void) {
  struct cme_Error *err =
      cme_error_create(88, "plain.c", "plain_func", 88, "Static message");

  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(88, err->code);
  TEST_ASSERT_EQUAL_STRING("plain.c", err->stack_symbols[0].source_file);
  TEST_ASSERT_EQUAL_STRING("plain_func", err->stack_symbols[0].source_func);
  TEST_ASSERT_EQUAL(88, err->stack_symbols[0].source_line);
  TEST_ASSERT_EQUAL_STRING("Static message", err->msg);

  cme_error_destroy(err);
}

void test_cme_error_dump_tmp(void) {
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
