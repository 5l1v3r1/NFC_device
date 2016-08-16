#pragma once

#include <vector>
#include "mainIInterface.h"
#include "IInterface.h"

class Observer
{
private:
	//this should be static variable
	//otherwise InotifyLoop cannot access it.
	std::vector<IInterface *> medium;
	inotifyFd _InotifyInfo;
	struct inotify_event *_Event;

	// static int CHILD_SUCCESSFUL;
	// static int CHILD_UNSUCCESSFUL;
	// static int CHILD_SIGNALED;
	// static int EXT_NOT_MATCH;
	// static int MESSAGE_REPEATED;
	// static int OTHER;

protected:
	int _State;
	static int globalCount;

public:
	Observer();
	Observer(char **);

	int initialize(void);
	int cleanup();
	void subscribe(IInterface *);
	void unSubscribe(IInterface *);
	void change(int);
	int notify_created();
	int notify_modified();
	int notify_deleted();

	int createThread();
	static void InotifyLoop(void *Observer_ptr);
};
