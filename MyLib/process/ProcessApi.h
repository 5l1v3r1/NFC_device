#include "../_base.h"
#include <signal.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define CHILD_SUCCESSFUL 0
#define CHILD_SIGNALED 1
#define ABNORMALLY_EXITED 2

int waitForProcess(int pid, int timeout, int wait);
#ifdef __cplusplus
}
#endif
