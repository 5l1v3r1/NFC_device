#pragma once

#include "main.h"
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

class Inotify
{
public:
	Inotify();

	void mainThread(char **argv);

public:
	inotifyFd _InotifyInfo;
};
