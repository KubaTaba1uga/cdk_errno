#ifndef LEVELS_H
#define LEVELS_H

#include "common.h"
#include "static.h"
#include <string.h>

#ifdef CME_ENABLE_BACKTRACE
static inline void inject_backtrace(cme_static_error_t err) {
  if (!err)
    return;

  void *symbols[CME_STACK_MAX];
  int count = cme_capture_backtrace(symbols, CME_STACK_MAX);
  if (count > 0) {
    memcpy(err->stack_symbols, symbols, sizeof(void *) * count);
    err->stack_length = count;
  }
}
#endif

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

static inline cme_static_error_t some_function_frameptr(int i) {
  return cme_return(level_frame_1(i));
}

// ========================== GLIBC backtrace based ==========================
static inline cme_static_error_t level_bt_5(int i) {
  cme_static_error_t err = cme_static_errorf(123, "err %d", i);
#ifdef CME_ENABLE_BACKTRACE
  inject_backtrace(err);
#endif
  return err;
}
static inline cme_static_error_t level_bt_4(int i) { return level_bt_5(i); }
static inline cme_static_error_t level_bt_3(int i) { return level_bt_4(i); }
static inline cme_static_error_t level_bt_2(int i) { return level_bt_3(i); }
static inline cme_static_error_t level_bt_1(int i) { return level_bt_2(i); }

static inline cme_static_error_t some_function_backtrace(int i) {
  return cme_return(level_bt_1(i));
}

#endif // LEVELS_H
