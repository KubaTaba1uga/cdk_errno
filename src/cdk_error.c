#include <threads.h>

#include "cdk_error.h"

thread_local struct cdk_Error *cdk_error;
thread_local struct cdk_Error _cdk_thread_error;
