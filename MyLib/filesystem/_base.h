#if !defined(_WIN32) || !defined(__WIN32__) || !defined(__WINDOWS__)
#include <unistd.h>
#include <dirent.h>
#include <sys/time.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

#define MAX_STRING_LEN 255
#define MAX_EXT_SIZE 64
