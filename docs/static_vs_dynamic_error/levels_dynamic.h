#ifndef LEVELS_DYNAMIC_H
#define LEVELS_DYNAMIC_H

#include "common.h"
#include "dynamic.h"
#include <string.h>

#ifdef CME_ENABLE_BACKTRACE
static inline void inject_backtrace_dynamic(cme_dynamic_error_t err) {
  if (!err)
    return;

  void *symbols[CME_STACK_MAX];
  int count = cme_capture_backtrace(symbols, CME_STACK_MAX);
  if (count > 0) {
    size_t sz = sizeof(void *) * count;
    err->stack_symbols = malloc(sz);
    if (err->stack_symbols) {
      memcpy(err->stack_symbols, symbols, sz);
      err->stack_length = count;
    }
  }
}
#endif

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

static inline cme_dynamic_error_t some_function_frameptr_dynamic(int i) {
  return cme_return(level_frame_dyn_1(i));
}

// ========================== GLIBC backtrace based ==========================
static inline cme_dynamic_error_t level_bt_dyn_5(int i) {
  cme_dynamic_error_t err = cme_dynamic_errorf(123, "dyn err %d", i);
#ifdef CME_ENABLE_BACKTRACE
  inject_backtrace_dynamic(err);
#endif
  return err;
}
static inline cme_dynamic_error_t level_bt_dyn_4(int i) {
  return level_bt_dyn_5(i);
}
static inline cme_dynamic_error_t level_bt_dyn_3(int i) {
  return level_bt_dyn_4(i);
}
static inline cme_dynamic_error_t level_bt_dyn_2(int i) {
  return level_bt_dyn_3(i);
}
static inline cme_dynamic_error_t level_bt_dyn_1(int i) {
  return level_bt_dyn_2(i);
}

static inline cme_dynamic_error_t some_function_backtrace_dynamic(int i) {
  return level_bt_dyn_1(i);
}

#endif // LEVELS_DYNAMIC_H
