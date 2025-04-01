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
  TEST_ASSERT_TRUE(err->stack_size > 0);
  TEST_ASSERT_NOT_NULL(err->stack_symbols);

  // Optional: Print a few frames for manual verification
  for (int i = 0; i < err->stack_size && i < 3; ++i) {
    printf("[backtrace #%d] %s\n", i, err->stack_symbols[i]);
  }

  cme_error_destroy(err);
}

void test_cme_error_dump_with_backtrace(void) {
  /* Create a dummy error structure */
  struct cme_Error err;
  err.code = 42;
  err.msg = "Temporary error message";
  err.source_file = "test_tmp.c";
  err.source_func = "test_tmp_func";
  err.source_line = 100;
  /* For testing, simulate one backtrace entry */
  err.stack_size = 1;
  char *dummy_symbols[] = {"[backtrace] dummy symbol"};
  err.stack_symbols = dummy_symbols;

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
  TEST_ASSERT_NOT_NULL(strstr(buf, "dummy symbol"));

  /* Clean up: remove the temporary file and free its path */
  remove(temp_file);
  free(temp_file);
}
