#include "c_minilib_error.h"
#include "common.h"
#include <unity.h>

void setUp() { cme_init(); }
void tearDown() { cme_destroy(); }

void test_ring_buffer_wraparound(void) {
  const int N = CME_RING_SIZE;
  cme_error_t slots[N + 1];
  for (int i = 0; i < N + 1; ++i) {
    slots[i] = cme_errorf(i, "err %d", i);
  }
  TEST_ASSERT_EQUAL_PTR(slots[0], slots[N]);
}

void test_ring_buffer_slot_isolation(void) {
  cme_error_t e1 = cme_error(1, "Error One");
  cme_error_t e2 = cme_error(2, "Error Two");

  TEST_ASSERT_NOT_NULL(e1);
  TEST_ASSERT_NOT_NULL(e2);
  TEST_ASSERT_NOT_EQUAL(e1, e2);
  TEST_ASSERT_EQUAL_STRING("Error One", e1->msg);
  TEST_ASSERT_EQUAL_STRING("Error Two", e2->msg);
}
