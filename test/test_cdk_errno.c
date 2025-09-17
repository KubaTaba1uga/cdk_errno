#include <errno.h>
#include <stdio.h>

#include "cdk_error.h"
#include "unity.h"

void test_integer_error_creation(void) {
  struct cdk_Error *err, base;
  err = cdk_error_int(&base, EINVAL, __FILE_NAME__, __func__, __LINE__);
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(EINVAL, err->code);
  TEST_ASSERT_EQUAL(cdk_ErrorType_INT, err->type);

  err = cdk_error_int(&base, EINPROGRESS, __FILE_NAME__, __func__, __LINE__);
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(EINPROGRESS, err->code);
  TEST_ASSERT_EQUAL(cdk_ErrorType_INT, err->type);
}

void test_string_error_creation(void) {
  struct cdk_Error *err, base;
  err = cdk_error_lstr(&base, EINVAL, __FILE_NAME__, __func__, __LINE__,
                       "Invalid user input");
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(EINVAL, err->code);
  TEST_ASSERT_EQUAL(cdk_ErrorType_STR, err->type);
  TEST_ASSERT_EQUAL_STRING("Invalid user input", base.msg);

  err = cdk_error_lstr(&base, EINPROGRESS, __FILE_NAME__, __func__, __LINE__,
                       "Shuting down in progress");
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(EINPROGRESS, err->code);
  TEST_ASSERT_EQUAL(cdk_ErrorType_STR, err->type);
  TEST_ASSERT_EQUAL_STRING("Shuting down in progress", base.msg);
}

void test_string_fmt_error_creation(void) {
#ifndef CDK_ERROR_OPTIMIZE
  struct cdk_Error *err, base;
  err = cdk_error_fstr(&base, EINVAL, __FILE_NAME__, __func__, __LINE__,
                       "Invalid user input: %d", EINVAL);
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(EINVAL, err->code);
  char expected[1024];
  sprintf(expected, "Invalid user input: %d", EINVAL);
  TEST_ASSERT_EQUAL_STRING(expected, base.msg);

  err = cdk_error_fstr(&base, EINPROGRESS, __FILE_NAME__, __func__, __LINE__,
                       "Downloading a file, cannot interrupt: %s",
                       "/opt/super_secret_file.txt");
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(EINPROGRESS, err->code);
  sprintf(expected, "Downloading a file, cannot interrupt: %s",
          "/opt/super_secret_file.txt");
  TEST_ASSERT_EQUAL_STRING(expected, base.msg);
#endif
}

void test_error_dump_to_str(void) {
  struct cdk_Error *err, base;
  err = cdk_errori(&base, 100);
  TEST_ASSERT_NOT_NULL(err);

  char buf[1024];
  cdk_error_dumps(err, sizeof(buf), buf);
  TEST_ASSERT_EQUAL_STRING(
      "====== ERROR DUMP ======\n"
      "Error code: 100\n"
      "Error desc: Network is down\n"
      "------------------------\n"
      " Backtrace:\n"
      "   [00] test_cdk_errno.c:test_error_dump_to_str:61\n",
      buf);

  err = cdk_errors(&base, 200, "Format error");
  TEST_ASSERT_NOT_NULL(err);

  cdk_error_dumps(err, sizeof(buf), buf);
  TEST_ASSERT_EQUAL_STRING(
      "====== ERROR DUMP ======\n"
      "Error code: 200\n"
      "Error desc: Unknown error 200\n"
      "------------------------\n"
      " Error msg: Format error\n"
      "------------------------\n"
      " Backtrace:\n"
      "   [00] test_cdk_errno.c:test_error_dump_to_str:75\n",
      buf);
}
