#include "c_minilib_error.h"
#include "common.h"
#include <string.h>
#include <unity.h>

void setUp() { cme_init(); }
void tearDown() { cme_destroy(); }

void test_literal_error_creation(void) {
  cme_error_t err = cme_error(42, "Simple literal error");
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(42, err->code);
  TEST_ASSERT_EQUAL_STRING("Simple literal error", err->msg);
  TEST_ASSERT_EQUAL_UINT8(1, err->frames_length);
}

void test_formatted_error_creation(void) {
  cme_error_t err = cme_errorf(5, "Failure with code %d", 5);
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(5, err->code);
  TEST_ASSERT_EQUAL_STRING("Failure with code 5", err->msg);
  TEST_ASSERT_EQUAL_UINT8(1, err->frames_length);
}

void test_error_dump_to_str(void) {
  cme_error_t err = cme_errorf(100, "Format error %d", 100);
  char buf[1024];
  int ret = cme_error_dump_to_str(err, sizeof(buf), buf);
  TEST_ASSERT_EQUAL_INT(0, ret);
  TEST_ASSERT_NOT_NULL(strstr(buf, "Format error 100"));
}

void test_error_dump_to_file(void) {
  cme_error_t err = cme_error(7, "File dump test");
  char *tmp = create_temp_file_path();
  TEST_ASSERT_NOT_NULL(tmp);
  int ret = cme_error_dump_to_file(err, tmp);
  TEST_ASSERT_EQUAL_INT(0, ret);

  FILE *fp = fopen(tmp, "r");
  TEST_ASSERT_NOT_NULL(fp);
  char buf[2048];
  size_t n = fread(buf, 1, sizeof(buf) - 1, fp);
  buf[n] = '\0';
  fclose(fp);

  TEST_ASSERT_NOT_NULL(strstr(buf, "File dump test"));
  remove(tmp);
  free(tmp);
}

void test_null_error_handling(void) {
  TEST_ASSERT_EQUAL_INT(EINVAL,
                        cme_error_dump_to_str(NULL, 100, (char[100]){}));
  TEST_ASSERT_EQUAL_INT(EINVAL, cme_error_dump_to_str((cme_error_t)1, 0, NULL));
}

void test_multiple_formatted_args(void) {
  cme_error_t err = cme_errorf(7, "Multiple %d %s %c", 123, "args", '!');
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(7, err->code);
  TEST_ASSERT_EQUAL_STRING("Multiple 123 args !", err->msg);
}

void test_no_format_string(void) {
  cme_error_t err = cme_error(88, "Static message");
  TEST_ASSERT_NOT_NULL(err);
  TEST_ASSERT_EQUAL(88, err->code);
  TEST_ASSERT_EQUAL_STRING("Static message", err->msg);
}
