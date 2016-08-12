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

using namespace std;

enum state
{
	created = 0, modified, deleted,
};

typedef struct _inotifyFd
{
	const char *path;
	int fd;
	int wd;
} inotifyFd;

class IInterface
{
public:
	IInterface() {
	}
	virtual int statusresponse(int);
	virtual int created(struct inotify_event *Event, inotifyFd InotifyInfo) = 0;
	virtual int modified(struct inotify_event *Event,
	        inotifyFd InotifyInfo) = 0;
	virtual int deleted(struct inotify_event *Event, inotifyFd InotifyInfo) = 0;
};

class Bluetooth: public IInterface
{
private:
	virtual int created(struct inotify_event *Event, inotifyFd InotifyInfo);
	virtual int modified(struct inotify_event *Event, inotifyFd InotifyInfo);
	virtual int deleted(struct inotify_event *Event, inotifyFd InotifyInfo);
	virtual int statusresponse(int);
};

class Wifi: public IInterface
{
private:
	virtual int created(struct inotify_event *Event, inotifyFd InotifyInfo);
	virtual int modified(struct inotify_event *Event, inotifyFd InotifyInfo);
	virtual int deleted(struct inotify_event *Event, inotifyFd InotifyInfo);
	virtual int statusresponse(int);
};
