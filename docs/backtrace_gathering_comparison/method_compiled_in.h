#ifndef METHOD_COMPILED_IN_H
#define METHOD_COMPILED_IN_H

#include <stddef.h>

#define TRACE_DEPTH 16

struct SingleTrace {
  const char *source_file;
  const char *source_func;
  int source_line;
};

typedef struct {
  struct SingleTrace *buffer;
  size_t buffer_len;
} compiled_trace_ctx_t;

#define ADD_TRACE(ctx)                                                         \
  (ctx)->buffer[(ctx)->buffer_len].source_file = __FILE__;                     \
  (ctx)->buffer[(ctx)->buffer_len].source_func = __func__;                     \
  (ctx)->buffer[(ctx)->buffer_len].source_line = __LINE__;                     \
  (ctx)->buffer_len++;

static void compiled_lvl16(compiled_trace_ctx_t *ctx) { ADD_TRACE(ctx); }
static void compiled_lvl15(compiled_trace_ctx_t *ctx) {
  ADD_TRACE(ctx);
  compiled_lvl16(ctx);
}
static void compiled_lvl14(compiled_trace_ctx_t *ctx) {
  ADD_TRACE(ctx);
  compiled_lvl15(ctx);
}
static void compiled_lvl13(compiled_trace_ctx_t *ctx) {
  ADD_TRACE(ctx);
  compiled_lvl14(ctx);
}
static void compiled_lvl12(compiled_trace_ctx_t *ctx) {
  ADD_TRACE(ctx);
  compiled_lvl13(ctx);
}
static void compiled_lvl11(compiled_trace_ctx_t *ctx) {
  ADD_TRACE(ctx);
  compiled_lvl12(ctx);
}
static void compiled_lvl10(compiled_trace_ctx_t *ctx) {
  ADD_TRACE(ctx);
  compiled_lvl11(ctx);
}
static void compiled_lvl9(compiled_trace_ctx_t *ctx) {
  ADD_TRACE(ctx);
  compiled_lvl10(ctx);
}
static void compiled_lvl8(compiled_trace_ctx_t *ctx) {
  ADD_TRACE(ctx);
  compiled_lvl9(ctx);
}
static void compiled_lvl7(compiled_trace_ctx_t *ctx) {
  ADD_TRACE(ctx);
  compiled_lvl8(ctx);
}
static void compiled_lvl6(compiled_trace_ctx_t *ctx) {
  ADD_TRACE(ctx);
  compiled_lvl7(ctx);
}
static void compiled_lvl5(compiled_trace_ctx_t *ctx) {
  ADD_TRACE(ctx);
  compiled_lvl6(ctx);
}
static void compiled_lvl4(compiled_trace_ctx_t *ctx) {
  ADD_TRACE(ctx);
  compiled_lvl5(ctx);
}
static void compiled_lvl3(compiled_trace_ctx_t *ctx) {
  ADD_TRACE(ctx);
  compiled_lvl4(ctx);
}
static void compiled_lvl2(compiled_trace_ctx_t *ctx) {
  ADD_TRACE(ctx);
  compiled_lvl3(ctx);
}
static void compiled_lvl1(compiled_trace_ctx_t *ctx) {
  ADD_TRACE(ctx);
  compiled_lvl2(ctx);
}

static inline void run_compiled_trace(struct SingleTrace buffer[TRACE_DEPTH],
                                      int *frames_out) {
  compiled_trace_ctx_t ctx = {.buffer_len = 0, .buffer = buffer};
  compiled_lvl1(&ctx);
  *frames_out = ctx.buffer_len;
}

#endif
