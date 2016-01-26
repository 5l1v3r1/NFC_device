#pragma once

#include "main.h"
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CHILD_SUCCESSFUL 0
#define CHILD_SIGNALED 1

class Inotify
{
public:
	Inotify();

	void mainThread(char **argv);

public:
	inotifyFd _InotifyInfo;
};
