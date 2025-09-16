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
}

/* void test_error_dump_to_str(void) { */
/*   cdk_errno = cdk_errorf(100, "Format error %d", 100); */

/*   char buf[1024]; */
/*   cdk_error_dump_to_str(cdk_errno, sizeof(buf), buf); */

/*   TEST_ASSERT_EQUAL_STRING("====== ERROR DUMP ======\n" */
/*                            "Error code: Network is down\n" */
/*                            "Error message: Format error 100\n" */
/*                            "------------------------\n", */
/*                            buf); */
/* } */
