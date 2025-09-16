#include <errno.h>
#include <stdio.h>

#include "cdk_error.h"
#include "unity.h"

void setUp() { cdk_errno = 0; }
void tearDown() { ; }

void test_integer_error_creation(void) {
  cdk_errno = cdk_error(EINVAL);
  TEST_ASSERT_NOT_NULL(cdk_errno);
  TEST_ASSERT_EQUAL(EINVAL, cdk_errno->code);

  cdk_errno = cdk_error(EINPROGRESS);
  TEST_ASSERT_NOT_NULL(cdk_errno);
  TEST_ASSERT_EQUAL(EINPROGRESS, cdk_errno->code);
}

void test_string_literal_error_creation(void) {
  cdk_errno = cdk_errors(EINVAL, "Invalid user input");
  TEST_ASSERT_NOT_NULL(cdk_errno);
  TEST_ASSERT_EQUAL(EINVAL, cdk_errno->code);
  TEST_ASSERT_EQUAL_STRING("Invalid user input", cdk_errno->msg);

  cdk_errno = cdk_errors(EINPROGRESS, "Downloading a file, cannot interrupt");
  TEST_ASSERT_NOT_NULL(cdk_errno);
  TEST_ASSERT_EQUAL(EINPROGRESS, cdk_errno->code);
  TEST_ASSERT_EQUAL_STRING("Downloading a file, cannot interrupt",
                           cdk_errno->msg);
}

void test_string_fmt_error_creation(void) {
  cdk_errno = cdk_errorf(EINVAL, "Invalid user input: %d", EINVAL);
  TEST_ASSERT_NOT_NULL(cdk_errno);
  TEST_ASSERT_EQUAL(EINVAL, cdk_errno->code);
  char expected[1024];
  sprintf(expected, "Invalid user input: %d", EINVAL);
  TEST_ASSERT_EQUAL_STRING(expected, cdk_errno->msg);

  cdk_errno =
      cdk_errorf(EINPROGRESS, "Downloading a file, cannot interrupt: %s",
                 "/opt/super_secret_file.txt");
  TEST_ASSERT_NOT_NULL(cdk_errno);
  TEST_ASSERT_EQUAL(EINPROGRESS, cdk_errno->code);
  sprintf(expected, "Downloading a file, cannot interrupt: %s",
          "/opt/super_secret_file.txt");
  TEST_ASSERT_EQUAL_STRING(expected, cdk_errno->msg);
}

void test_error_dump_to_str(void) {
  cdk_errno = cdk_errorf(100, "Format error %d", 100);

  char buf[1024];
  cdk_error_dump_to_str(cdk_errno, sizeof(buf), buf);

  TEST_ASSERT_EQUAL_STRING("====== ERROR DUMP ======\n"
                           "Error code: Network is down\n"
                           "Error message: Format error 100\n"
                           "------------------------\n",
                           buf);
}
