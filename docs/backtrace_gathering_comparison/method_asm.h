#ifndef METHOD_ASM_H
#define METHOD_ASM_H

#include <stddef.h>

#define TRACE_DEPTH 16

typedef struct {
  void **buffer;
  size_t size;
  int frames;
} asm_trace_ctx_t;

static inline void capture_ip_asm(void **dst) {
#if defined(__x86_64__)
  __asm__ volatile("lea (%%rip), %0" : "=r"(*dst));
#elif defined(__i386__)
  __asm__ volatile("call 1f\n1: pop %0" : "=r"(*dst));
#elif defined(__aarch64__)
  __asm__ volatile("adr %0, ." : "=r"(*dst));
#elif defined(__arm__)
  __asm__ volatile("adr %0, here\nhere:" : "=r"(*dst));
#else
#error "Unsupported architecture"
#endif
}

// Level functions
static void asm_lvl16(asm_trace_ctx_t *ctx) {
  capture_ip_asm(&ctx->buffer[ctx->frames++]);
}
static void asm_lvl15(asm_trace_ctx_t *ctx) {
  capture_ip_asm(&ctx->buffer[ctx->frames++]);
  asm_lvl16(ctx);
}
static void asm_lvl14(asm_trace_ctx_t *ctx) {
  capture_ip_asm(&ctx->buffer[ctx->frames++]);
  asm_lvl15(ctx);
}
static void asm_lvl13(asm_trace_ctx_t *ctx) {
  capture_ip_asm(&ctx->buffer[ctx->frames++]);
  asm_lvl14(ctx);
}
static void asm_lvl12(asm_trace_ctx_t *ctx) {
  capture_ip_asm(&ctx->buffer[ctx->frames++]);
  asm_lvl13(ctx);
}
static void asm_lvl11(asm_trace_ctx_t *ctx) {
  capture_ip_asm(&ctx->buffer[ctx->frames++]);
  asm_lvl12(ctx);
}
static void asm_lvl10(asm_trace_ctx_t *ctx) {
  capture_ip_asm(&ctx->buffer[ctx->frames++]);
  asm_lvl11(ctx);
}
static void asm_lvl9(asm_trace_ctx_t *ctx) {
  capture_ip_asm(&ctx->buffer[ctx->frames++]);
  asm_lvl10(ctx);
}
static void asm_lvl8(asm_trace_ctx_t *ctx) {
  capture_ip_asm(&ctx->buffer[ctx->frames++]);
  asm_lvl9(ctx);
}
static void asm_lvl7(asm_trace_ctx_t *ctx) {
  capture_ip_asm(&ctx->buffer[ctx->frames++]);
  asm_lvl8(ctx);
}
static void asm_lvl6(asm_trace_ctx_t *ctx) {
  capture_ip_asm(&ctx->buffer[ctx->frames++]);
  asm_lvl7(ctx);
}
static void asm_lvl5(asm_trace_ctx_t *ctx) {
  capture_ip_asm(&ctx->buffer[ctx->frames++]);
  asm_lvl6(ctx);
}
static void asm_lvl4(asm_trace_ctx_t *ctx) {
  capture_ip_asm(&ctx->buffer[ctx->frames++]);
  asm_lvl5(ctx);
}
static void asm_lvl3(asm_trace_ctx_t *ctx) {
  capture_ip_asm(&ctx->buffer[ctx->frames++]);
  asm_lvl4(ctx);
}
static void asm_lvl2(asm_trace_ctx_t *ctx) {
  capture_ip_asm(&ctx->buffer[ctx->frames++]);
  asm_lvl3(ctx);
}
static void asm_lvl1(asm_trace_ctx_t *ctx) {
  capture_ip_asm(&ctx->buffer[ctx->frames++]);
  asm_lvl2(ctx);
}

static inline void run_asm_trace(void **buffer, size_t size, int *frames_out) {
  asm_trace_ctx_t ctx = {.buffer = buffer, .size = size, .frames = 0};
  asm_lvl1(&ctx);
  *frames_out = ctx.frames;
}

#endif
