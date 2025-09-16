#include <threads.h>

#include "cdk_errno.h"

thread_local struct cdk_Errno *cdk_errno;
thread_local struct cdk_Errno cdk_thread_error;
