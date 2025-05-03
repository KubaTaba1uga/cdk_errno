#include "c_minilib_error.h"

struct cme_Error *cme_ringbuf = NULL;
uint32_t cme_ringbuf_i = 0;
struct cme_Error _cme_error_fallback = {0};
