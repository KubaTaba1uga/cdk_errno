#ifndef METHOD_MANUAL_H
#define METHOD_MANUAL_H

#include <stddef.h>

#define TRACE_DEPTH 16

typedef struct {
  void **buffer;
  size_t size;
  int frames;
} manual_trace_ctx_t;

static void manual_lvl16(manual_trace_ctx_t *ctx) {
  ctx->buffer[ctx->frames++] =
      __builtin_extract_return_addr(__builtin_return_address(0));
}
static void manual_lvl15(manual_trace_ctx_t *ctx) {
  ctx->buffer[ctx->frames++] =
      __builtin_extract_return_addr(__builtin_return_address(0));
  manual_lvl16(ctx);
}
static void manual_lvl14(manual_trace_ctx_t *ctx) {
  ctx->buffer[ctx->frames++] =
      __builtin_extract_return_addr(__builtin_return_address(0));
  manual_lvl15(ctx);
}
static void manual_lvl13(manual_trace_ctx_t *ctx) {
  ctx->buffer[ctx->frames++] =
      __builtin_extract_return_addr(__builtin_return_address(0));
  manual_lvl14(ctx);
}
static void manual_lvl12(manual_trace_ctx_t *ctx) {
  ctx->buffer[ctx->frames++] =
      __builtin_extract_return_addr(__builtin_return_address(0));
  manual_lvl13(ctx);
}
static void manual_lvl11(manual_trace_ctx_t *ctx) {
  ctx->buffer[ctx->frames++] =
      __builtin_extract_return_addr(__builtin_return_address(0));
  manual_lvl12(ctx);
}
static void manual_lvl10(manual_trace_ctx_t *ctx) {
  ctx->buffer[ctx->frames++] =
      __builtin_extract_return_addr(__builtin_return_address(0));
  manual_lvl11(ctx);
}
static void manual_lvl9(manual_trace_ctx_t *ctx) {
  ctx->buffer[ctx->frames++] =
      __builtin_extract_return_addr(__builtin_return_address(0));
  manual_lvl10(ctx);
}
static void manual_lvl8(manual_trace_ctx_t *ctx) {
  ctx->buffer[ctx->frames++] =
      __builtin_extract_return_addr(__builtin_return_address(0));
  manual_lvl9(ctx);
}
static void manual_lvl7(manual_trace_ctx_t *ctx) {
  ctx->buffer[ctx->frames++] =
      __builtin_extract_return_addr(__builtin_return_address(0));
  manual_lvl8(ctx);
}
static void manual_lvl6(manual_trace_ctx_t *ctx) {
  ctx->buffer[ctx->frames++] =
      __builtin_extract_return_addr(__builtin_return_address(0));
  manual_lvl7(ctx);
}
static void manual_lvl5(manual_trace_ctx_t *ctx) {
  ctx->buffer[ctx->frames++] =
      __builtin_extract_return_addr(__builtin_return_address(0));
  manual_lvl6(ctx);
}
static void manual_lvl4(manual_trace_ctx_t *ctx) {
  ctx->buffer[ctx->frames++] =
      __builtin_extract_return_addr(__builtin_return_address(0));
  manual_lvl5(ctx);
}
static void manual_lvl3(manual_trace_ctx_t *ctx) {
  ctx->buffer[ctx->frames++] =
      __builtin_extract_return_addr(__builtin_return_address(0));
  manual_lvl4(ctx);
}
static void manual_lvl2(manual_trace_ctx_t *ctx) {
  ctx->buffer[ctx->frames++] =
      __builtin_extract_return_addr(__builtin_return_address(0));
  manual_lvl3(ctx);
}
static void manual_lvl1(manual_trace_ctx_t *ctx) {
  ctx->buffer[ctx->frames++] =
      __builtin_extract_return_addr(__builtin_return_address(0));
  manual_lvl2(ctx);
}

static inline void run_manual_trace(void **buffer, size_t size,
                                    int *frames_out) {
  manual_trace_ctx_t ctx = {.buffer = buffer, .size = size, .frames = 0};
  manual_lvl1(&ctx);
  *frames_out = ctx.frames;
}

#endif
