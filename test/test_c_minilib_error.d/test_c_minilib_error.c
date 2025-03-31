#include <string.h>

#include <unity.h>

#include "c_minilib_error.h"

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
