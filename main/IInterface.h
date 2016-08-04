#pragma once

#include <vector>
#include "../MyLib/_base.h"
#include "../MyLib/thread/fast_mutex.h"
#include "../MyLib/thread/tinythread.h"
#include "../MyLib/filesystem/FileApi.h"
#include "mainIInterface.h"

/*MACROS*/
#define MAX_StateS		1024 /*Max. number of events to process at one go*/
#define LEN_NAME		16 /*Assuming that the length of the filename won't exceed 16 bytes*/
#define EVENT_SIZE  	( sizeof (struct inotify_event) ) /*size of one event*/
#define BUF_LEN			( MAX_StateS * ( EVENT_SIZE + LEN_NAME )) /*buffer to store the data of events*/
#define DEFAULT_INOTIFY_PATH "./"

enum state
{
	created = 0,
	modified,
	deleted,
};

typedef struct _inotifyFd
{
	const char *path;
	int fd;
	int wd;
} inotifyFd;

class IInterface
{
protected:
	int _State;
public:
	IInterface() { }
	virtual void created(int _State) = 0;
	virtual void modified(int _State) = 0;
	virtual void deleted(int _State) = 0;
};

class Bluetooth : public IInterface
{
	virtual void created(int i);
	virtual void modified(int i);
	virtual void deleted(int i);
};

class Wifi : public IInterface
{
	virtual void created(int i);
	virtual void modified(int i);
	virtual void deleted(int i);
};

class Observer
{
private:
	//this should be static variable
	//otherwise InotifyLoop cannot access it.
	std::vector<IInterface *> medium;
	inotifyFd _InotifyInfo;
	struct inotify_event *_Event;

protected:
	int _State;
	static int globalCount;

public:
	Observer();
	Observer(char **);

	int initialize(void);
	int cleanup();
	void Register(IInterface *);
	void Change(int );

	void Notify_created();
	void Notify_modified();
	void Notify_deleted();

	int CreateThread();
	static void InotifyLoop(void *Observer_ptr);
};
