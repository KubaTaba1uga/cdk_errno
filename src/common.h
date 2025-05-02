#ifndef CME_COMMON_H
#define CME_COMMON_H

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/**
 * Simplified, safe wrapper around snprintf.
 *
 * @param buffer       Destination buffer
 * @param buffer_size  Size of buffer in bytes
 * @param format       printf-style format string
 * @param ...          Format arguments
 * @return Number of characters written (excluding null), or -1 on error or
 * truncation.
 */
static inline int cme_sprintf(char *buffer, size_t buffer_size,
                              const char *format, ...) {
  if (!buffer || buffer_size == 0 || !format) {
    return -1;
  }

  va_list args;
  va_start(args, format);
  int result = vsnprintf(buffer, buffer_size, format, args);
  va_end(args);

  if (result < 0) {
    /* Encoding or other error */
    buffer[0] = '\0';
    return -1;
  }
  if ((size_t)result >= buffer_size) {
    /* Truncated: buffer is already null-terminated at buffer[buffer_size-1] */
    return -1;
  }

  /* Success: buffer[result] is null-terminated */
  return result;
}

#endif // CME_COMMON_H
