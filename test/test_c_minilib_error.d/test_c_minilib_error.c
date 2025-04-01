#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <unity.h>

#include "c_minilib_error.h"
#include "common.h"

void test_create_error_success(void) {
  struct cme_Error *err = cme_error_create(42, "test.c", "test_func", 123,
                                           "Failed with code %d", 42);

  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(42, err->code);
  TEST_ASSERT_EQUAL_STRING("test.c", err->source_file);
  TEST_ASSERT_EQUAL_STRING("test_func", err->source_func);
  TEST_ASSERT_EQUAL(123, err->source_line);
  TEST_ASSERT_NOT_NULL(err->msg);
  TEST_ASSERT_EQUAL_STRING("Failed with code 42", err->msg);

  cme_error_destroy(err);
}

void test_create_error_null_source(void) {
  struct cme_Error *err = cme_error_create(99, NULL, NULL, 0, "Just a message");

  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(99, err->code);
  TEST_ASSERT_EQUAL(0, err->source_line);
  TEST_ASSERT_NULL(err->source_file);
  TEST_ASSERT_NULL(err->source_func);
  TEST_ASSERT_NOT_NULL(err->msg);
  TEST_ASSERT_EQUAL_STRING("Just a message", err->msg);

  cme_error_destroy(err);
}

void test_create_error_invalid_format(void) {
  struct cme_Error *err = cme_error_create(1, "x.c", "func", 1, NULL);

  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(1, err->code);
  cme_error_destroy(err);
}

void test_destroy_null(void) {
  cme_error_destroy(NULL); // Should not crash
}

void test_create_error_multiple_format_args(void) {
  struct cme_Error *err =
      cme_error_create(7, "multi.c", "multi_func", 77, "Error code %d at %s:%d",
                       7, "multi.c", 77);

  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(7, err->code);
  TEST_ASSERT_EQUAL_STRING("multi.c", err->source_file);
  TEST_ASSERT_EQUAL_STRING("multi_func", err->source_func);
  TEST_ASSERT_EQUAL(77, err->source_line);
  TEST_ASSERT_NOT_NULL(err->msg);
  TEST_ASSERT_EQUAL_STRING("Error code 7 at multi.c:77", err->msg);

  cme_error_destroy(err);
}

void test_create_error_no_format_string(void) {
  struct cme_Error *err =
      cme_error_create(88, "plain.c", "plain_func", 88, "Static message");

  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(88, err->code);
  TEST_ASSERT_EQUAL_STRING("plain.c", err->source_file);
  TEST_ASSERT_EQUAL_STRING("plain_func", err->source_func);
  TEST_ASSERT_EQUAL(88, err->source_line);
  TEST_ASSERT_NOT_NULL(err->msg);
  TEST_ASSERT_EQUAL_STRING("Static message", err->msg);

  cme_error_destroy(err);
}

void test_cme_error_dump_tmp(void) {
  /* Create a dummy error structure */
  struct cme_Error err;
  err.code = 42;
  err.msg = "Temporary error message";
  err.source_file = "test_tmp.c";
  err.source_func = "test_tmp_func";
  err.source_line = 100;
  err.stack_size = 0;
  err.stack_symbols = NULL;

  /* Generate a temporary file path in /tmp */
  char *temp_file = create_temp_file_path();
  TEST_ASSERT_NOT_NULL_MESSAGE(temp_file, "Failed to create temp file path");

  /* Dump error to the temporary file */
  int ret = cme_error_dump(&err, temp_file);
  TEST_ASSERT_EQUAL_INT_MESSAGE(0, ret,
                                "cme_error_dump() should return 0 on success");

  /* Open the temporary file for reading */
  FILE *fp = fopen(temp_file, "r");
  TEST_ASSERT_NOT_NULL_MESSAGE(fp, "Dump file should be created");

  /* Read the file's contents */
  char buf[2048];
  size_t n = fread(buf, 1, sizeof(buf) - 1, fp);
  buf[n] = '\0';
  fclose(fp);

  /* Validate expected content */
  TEST_ASSERT_NOT_NULL(strstr(buf, "====== ERROR DUMP ======"));
  {
    char code_str[64];
    sprintf(code_str, "Error code: %d", err.code);
    TEST_ASSERT_NOT_NULL(strstr(buf, code_str));
  }
  TEST_ASSERT_NOT_NULL(strstr(buf, err.msg));
  TEST_ASSERT_NOT_NULL(strstr(buf, err.source_file));
  TEST_ASSERT_NOT_NULL(strstr(buf, "Src line: 100"));
  TEST_ASSERT_NOT_NULL(strstr(buf, err.source_func));

  /* Clean up: remove the temporary file and free its path */
  remove(temp_file);
  free(temp_file);
}
