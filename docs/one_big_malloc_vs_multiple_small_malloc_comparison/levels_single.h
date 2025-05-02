#ifndef LEVELS_H
#define LEVELS_H
#include <string.h>

#include "common.h"
#include "method_single.h"

// ========================== Frame-pointer based ==========================
static inline cme_static_error_t level_frame_5(int i) {
  return cme_return(cme_static_errorf(123, "err %d", i));
}
static inline cme_static_error_t level_frame_4(int i) {
  return cme_return(level_frame_5(i));
}
static inline cme_static_error_t level_frame_3(int i) {
  return cme_return(level_frame_4(i));
}
static inline cme_static_error_t level_frame_2(int i) {
  return cme_return(level_frame_3(i));
}
static inline cme_static_error_t level_frame_1(int i) {
  return cme_return(level_frame_2(i));
}
static inline cme_static_error_t test_nested_error(int i) {
  return cme_return(level_frame_1(i));
}

#endif // LEVELS_H
