#include "cdk_error.h"
thread_local cdk_error_t cdk_errno = NULL;
thread_local struct cdk_Error cdk_hidden_errno = {0};
