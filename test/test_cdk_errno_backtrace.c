#include <errno.h>

#include "cdk_error.h"
#include "unity.h"

_Thread_local cdk_error_t cdk_errno = NULL;
_Thread_local struct cdk_Error cdk_hidden_errno = {0};

void test_unity(void) { TEST_PASS_MESSAGE("Unity is working."); }

void test_backtrace_creation(void) {
  cdk_errno = cdk_errnoi(EINVAL);
  TEST_ASSERT_NOT_NULL(cdk_errno);
  TEST_ASSERT_EQUAL(EINVAL, cdk_errno->code);
  TEST_ASSERT_EQUAL(1, cdk_errno->eframes_len);
  TEST_ASSERT_EQUAL_STRING("test_cdk_errno_backtrace.c",
                           cdk_errno->eframes[0].file);
  TEST_ASSERT_EQUAL_STRING("test_backtrace_creation",
                           cdk_errno->eframes[0].func);
  TEST_ASSERT_EQUAL(12, cdk_errno->eframes[0].line);

  cdk_ewrap();
  TEST_ASSERT_EQUAL(2, cdk_errno->eframes_len);
  TEST_ASSERT_EQUAL_STRING("test_cdk_errno_backtrace.c",
                           cdk_errno->eframes[1].file);
  TEST_ASSERT_EQUAL_STRING("test_backtrace_creation",
                           cdk_errno->eframes[1].func);
  TEST_ASSERT_EQUAL(22, cdk_errno->eframes[1].line);

  cdk_ewrap();
  TEST_ASSERT_EQUAL(3, cdk_errno->eframes_len);
  TEST_ASSERT_EQUAL_STRING("test_cdk_errno_backtrace.c",
                           cdk_errno->eframes[2].file);
  TEST_ASSERT_EQUAL_STRING("test_backtrace_creation",
                           cdk_errno->eframes[2].func);
  TEST_ASSERT_EQUAL(30, cdk_errno->eframes[2].line);

  cdk_errno = cdk_errnoi(EINPROGRESS);
  TEST_ASSERT_NOT_NULL(cdk_errno);
  TEST_ASSERT_EQUAL(EINPROGRESS, cdk_errno->code);
  TEST_ASSERT_EQUAL(1, cdk_errno->eframes_len);
  TEST_ASSERT_EQUAL_STRING("test_cdk_errno_backtrace.c",
                           cdk_errno->eframes[0].file);
  TEST_ASSERT_EQUAL_STRING("test_backtrace_creation",
                           cdk_errno->eframes[0].func);
  TEST_ASSERT_EQUAL(38, cdk_errno->eframes[0].line);
}

int my_nested_func(int number) {
  if (number >= 0) {
    return number;
  }

  cdk_errno = cdk_errnoi(EINVAL);

  return -1;
}

int my_failing_func(void) {
  int number = -10;

  int result = my_nested_func(number);
  if (result < 0) {
    return cdk_ereturn(result);
  }

  return result;
}

void test_erereturn_success(void) {
  int res = my_failing_func();

  TEST_ASSERT_EQUAL(-1, res);
  TEST_ASSERT_EQUAL(2, cdk_errno->eframes_len);
  TEST_ASSERT_EQUAL_STRING("test_cdk_errno_backtrace.c",
                           cdk_errno->eframes[0].file);
  TEST_ASSERT_EQUAL_STRING("my_nested_func", cdk_errno->eframes[0].func);
  TEST_ASSERT_EQUAL(54, cdk_errno->eframes[0].line);
  TEST_ASSERT_EQUAL_STRING("test_cdk_errno_backtrace.c",
                           cdk_errno->eframes[1].file);
  TEST_ASSERT_EQUAL_STRING("my_failing_func", cdk_errno->eframes[1].func);
  TEST_ASSERT_EQUAL(64, cdk_errno->eframes[1].line);
}
