#pragma once

#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CHILD_SUCCESSFUL 1
#define CHILD_UNSUCCESSFUL 2
#define CHILD_SIGNALED 3
#define EXT_NOT_MATCH 4
#define MESSAGE_REPEATED 5
#define SEND_DATA_AGAIN 8
#define OTHER 9

#define DEFAULT_INOTIFY_PATH "./"
