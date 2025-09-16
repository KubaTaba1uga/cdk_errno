#include <errno.h>

#include "cdk_error.h"
#include "unity.h"

void test_unity(void) { TEST_PASS_MESSAGE("Unity is working."); }

void test_backtrace_creation(void) {
  cdk_errno = cdk_error(EINVAL);
  TEST_ASSERT_NOT_NULL(cdk_errno);
  TEST_ASSERT_EQUAL(EINVAL, cdk_errno->code);
  TEST_ASSERT_EQUAL(1, cdk_errno->btraces_len);
  TEST_ASSERT_EQUAL_STRING("../test/test_cdk_errno_backtrace.c",
                           cdk_errno->btraces[0].file);
  TEST_ASSERT_EQUAL_STRING("test_backtrace_creation",
                           cdk_errno->btraces[0].func);
  TEST_ASSERT_EQUAL(9, cdk_errno->btraces[0].line);

  cdk_ewrap(cdk_errno);
  TEST_ASSERT_EQUAL(2, cdk_errno->btraces_len);
  TEST_ASSERT_EQUAL_STRING("../test/test_cdk_errno_backtrace.c",
                           cdk_errno->btraces[1].file);
  TEST_ASSERT_EQUAL_STRING("test_backtrace_creation",
                           cdk_errno->btraces[1].func);
  TEST_ASSERT_EQUAL(19, cdk_errno->btraces[1].line);

  cdk_ewrap(cdk_errno);
  TEST_ASSERT_EQUAL(3, cdk_errno->btraces_len);
  TEST_ASSERT_EQUAL_STRING("../test/test_cdk_errno_backtrace.c",
                           cdk_errno->btraces[2].file);
  TEST_ASSERT_EQUAL_STRING("test_backtrace_creation",
                           cdk_errno->btraces[2].func);
  TEST_ASSERT_EQUAL(27, cdk_errno->btraces[2].line);

  cdk_errno = cdk_error(EINPROGRESS);
  TEST_ASSERT_NOT_NULL(cdk_errno);
  TEST_ASSERT_EQUAL(EINPROGRESS, cdk_errno->code);
  TEST_ASSERT_EQUAL(1, cdk_errno->btraces_len);
  TEST_ASSERT_EQUAL_STRING("../test/test_cdk_errno_backtrace.c",
                           cdk_errno->btraces[0].file);
  TEST_ASSERT_EQUAL_STRING("test_backtrace_creation",
                           cdk_errno->btraces[0].func);
  TEST_ASSERT_EQUAL(35, cdk_errno->btraces[0].line);
}

int my_nested_func(int number) {
  if (number >= 0) {
    return number;
  }

  cdk_errno = cdk_error(EINVAL);

  return -1;
}

int my_failing_func(void) {
  int number = -10;

  int result = my_nested_func(number);
  if (result < 0) {
    return cdk_ereturn(result, cdk_errno);
  }

  return result;
}

void test_erereturn_success(void) {
  int res = my_failing_func();

  TEST_ASSERT_EQUAL(-1, res);
  TEST_ASSERT_EQUAL(2, cdk_errno->btraces_len);
  TEST_ASSERT_EQUAL_STRING("../test/test_cdk_errno_backtrace.c",
                           cdk_errno->btraces[0].file);
  TEST_ASSERT_EQUAL_STRING("my_nested_func", cdk_errno->btraces[0].func);
  TEST_ASSERT_EQUAL(51, cdk_errno->btraces[0].line);
  TEST_ASSERT_EQUAL_STRING("../test/test_cdk_errno_backtrace.c",
                           cdk_errno->btraces[1].file);
  TEST_ASSERT_EQUAL_STRING("my_failing_func", cdk_errno->btraces[1].func);
  TEST_ASSERT_EQUAL(61, cdk_errno->btraces[1].line);
}

void test_ewrap_success(void) {
  int res = my_failing_func();

  TEST_ASSERT_EQUAL(-1, res);
  TEST_ASSERT_EQUAL(2, cdk_errno->btraces_len);
  TEST_ASSERT_EQUAL_STRING("../test/test_cdk_errno_backtrace.c",
                           cdk_errno->btraces[0].file);
  TEST_ASSERT_EQUAL_STRING("my_nested_func", cdk_errno->btraces[0].func);
  TEST_ASSERT_EQUAL(51, cdk_errno->btraces[0].line);
  TEST_ASSERT_EQUAL_STRING("../test/test_cdk_errno_backtrace.c",
                           cdk_errno->btraces[1].file);
  TEST_ASSERT_EQUAL_STRING("my_failing_func", cdk_errno->btraces[1].func);
  TEST_ASSERT_EQUAL(61, cdk_errno->btraces[1].line);
}
