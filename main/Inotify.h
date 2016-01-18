#pragma once

#include "main.h"
#include <iostream>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SEND_FAIL -1
#define SEND_SUCCESS 0

class Inotify
{
public:
	Inotify();

	void mainThread(char **argv);

public:
	inotifyFd _InotifyInfo;
};
