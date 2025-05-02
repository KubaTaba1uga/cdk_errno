#ifndef LEVELS_DYNAMIC_H
#define LEVELS_DYNAMIC_H
#include <string.h>

#include "common.h"
#include "method_multiple.h"

// ========================== Frame-pointer based ==========================
static inline cme_dynamic_error_t level_frame_dyn_5(int i) {
  return cme_return(cme_dynamic_errorf(123, "dyn err %d", i));
}
static inline cme_dynamic_error_t level_frame_dyn_4(int i) {
  return cme_return(level_frame_dyn_5(i));
}
static inline cme_dynamic_error_t level_frame_dyn_3(int i) {
  return cme_return(level_frame_dyn_4(i));
}
static inline cme_dynamic_error_t level_frame_dyn_2(int i) {
  return cme_return(level_frame_dyn_3(i));
}
static inline cme_dynamic_error_t level_frame_dyn_1(int i) {
  return cme_return(level_frame_dyn_2(i));
}

static inline cme_dynamic_error_t test_nested_error(int i) {
  return cme_return(level_frame_dyn_1(i));
}

#endif // LEVELS_DYNAMIC_H
