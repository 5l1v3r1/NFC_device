#pragma once

#include "main.h"
#include <iostream>
class Inotify
{
public:
	Inotify();

	void mainThread(char **argv);

public:
	inotifyFd _InotifyInfo;
};
