#include "c_minilib_error.h"
#include "common.h"
#include <unity.h>

void test_ring_buffer_wraparound(void) {
  // Configure ring buffer to small size for testing
  cme_configure(
      &(struct cme_Settings){.ring_size = 4, .is_ring_growable = false});
  cme_init();

  // Allocate exactly `ring_size` errors
  struct cme_Error *errs[4];
  for (int i = 0; i < 4; ++i) {
    char msg[32];
    snprintf(msg, sizeof(msg), "Error %d", i);
    errs[i] = cme_error_create(100 + i, "wrap.c", "wrap_func", i, msg);
    TEST_ASSERT_NOT_NULL(errs[i]);
    TEST_ASSERT_EQUAL(100 + i, errs[i]->code);
    TEST_ASSERT_EQUAL_STRING(msg, errs[i]->msg);
  }

  // Allocate one more to cause wraparound
  struct cme_Error *wrap_err =
      cme_error_create(200, "wrap.c", "wrap_func", 999, "WRAPPED");
  TEST_ASSERT_NOT_NULL(wrap_err);

  // It should overwrite the first error (errs[0])
  TEST_ASSERT_EQUAL_PTR(wrap_err, errs[0]);
  TEST_ASSERT_EQUAL(200, wrap_err->code);
  TEST_ASSERT_EQUAL_STRING("WRAPPED", wrap_err->msg);

  cme_destroy();
}

void test_ring_buffer_isolation_between_slots(void) {
  cme_configure(
      &(struct cme_Settings){.ring_size = 2, .is_ring_growable = false});
  cme_init();

  struct cme_Error *e1 = cme_error_create(1, "iso.c", "func", 1, "E1");
  struct cme_Error *e2 = cme_error_create(2, "iso.c", "func", 2, "E2");

  // Ensure different slots
  TEST_ASSERT_NOT_NULL(e1);
  TEST_ASSERT_NOT_NULL(e2);
  TEST_ASSERT_NOT_EQUAL(e1, e2);

  // E1 and E2 contain isolated data
  TEST_ASSERT_EQUAL(1, e1->code);
  TEST_ASSERT_EQUAL_STRING("E1", e1->msg);
  TEST_ASSERT_EQUAL(2, e2->code);
  TEST_ASSERT_EQUAL_STRING("E2", e2->msg);

  cme_destroy();
}
