#ifndef METHOD_BACKTRACE_H
#define METHOD_BACKTRACE_H

#include <execinfo.h>
#include <stddef.h>

static void lvl16(void **buffer, size_t size, int *frames) {
  *frames = backtrace(buffer, size);
}
static void lvl15(void **buffer, size_t size, int *frames) {
  lvl16(buffer, size, frames);
}
static void lvl14(void **buffer, size_t size, int *frames) {
  lvl15(buffer, size, frames);
}
static void lvl13(void **buffer, size_t size, int *frames) {
  lvl14(buffer, size, frames);
}
static void lvl12(void **buffer, size_t size, int *frames) {
  lvl13(buffer, size, frames);
}
static void lvl11(void **buffer, size_t size, int *frames) {
  lvl12(buffer, size, frames);
}
static void lvl10(void **buffer, size_t size, int *frames) {
  lvl11(buffer, size, frames);
}
static void lvl9(void **buffer, size_t size, int *frames) {
  lvl10(buffer, size, frames);
}
static void lvl8(void **buffer, size_t size, int *frames) {
  lvl9(buffer, size, frames);
}
static void lvl7(void **buffer, size_t size, int *frames) {
  lvl8(buffer, size, frames);
}
static void lvl6(void **buffer, size_t size, int *frames) {
  lvl7(buffer, size, frames);
}
static void lvl5(void **buffer, size_t size, int *frames) {
  lvl6(buffer, size, frames);
}
static void lvl4(void **buffer, size_t size, int *frames) {
  lvl5(buffer, size, frames);
}
static void lvl3(void **buffer, size_t size, int *frames) {
  lvl4(buffer, size, frames);
}
static void lvl2(void **buffer, size_t size, int *frames) {
  lvl3(buffer, size, frames);
}
static void lvl1(void **buffer, size_t size, int *frames) {
  lvl2(buffer, size, frames);
}

// Public entry
static inline void run_backtrace_posix(void **buffer, size_t size,
                                       int *frames) {
  lvl1(buffer, size, frames);
}

#endif
