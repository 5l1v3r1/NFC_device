//// Reference: http://www.opensourceforu.com/2011/04/getting-started-with-inotify/
//// Thread library TinyThread++ 1.1
#if not defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#include <sys/inotify.h>
#include <unistd.h>
#endif

#include <iostream>
#include <list>
#include "monitor.h"
#include "../MyLib/thread/fast_mutex.h"
#include "../MyLib/thread/tinythread.h"
#include "../MyLib/filesystem/FileApi.h"
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>

#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>


using namespace std;
using namespace tthread;

#ifdef TEST_MUTEX
// Mutex + global count variable
mutex gMutex;
fast_mutex gFastMutex;
int gCount;
#endif

#define MAX_EVENTS 1024 /*Max. number of events to process at one go*/
#define LEN_NAME 16 /*Assuming that the length of the filename won't exceed 16 bytes*/
#define EVENT_SIZE  ( sizeof (struct inotify_event) ) /*size of one event*/
#define BUF_LEN     ( MAX_EVENTS * ( EVENT_SIZE + LEN_NAME )) /*buffer to store the data of events*/

#ifdef TEST_MUTEX
// Thread function: Mutex locking
void ThreadLock(void * aArg)
{
	for (int i = 0; i < 2; ++i)
	{
		lock_guard<mutex> lock(gMutex);
		++gCount;
	}
}
#endif

bool createDirectory(const char* path, __mode_t mode)
{
	mkdir(path, mode);
	return 0;
}
// This is the child thread function
void mainThread(void * aArg)
{
	int length, i = 0;
	char buffer[BUF_LEN];
	int fd = *(int *) aArg;
	/* do it forever*/
	while (1)
	{
		i = 0;
		length = read(fd, buffer, BUF_LEN);

		if (length < 0)
		{
			cerr << "read" << endl;
		}

		while (i < length)
		{
			struct inotify_event *event = (struct inotify_event *) &buffer[i];
			if (event->len)
			{
				if (event->mask & IN_CREATE)
				{
					if (event->mask & IN_ISDIR)
					{
						cout << "The directory " << event->name
						        << " was Created.\n" << endl;

					}
					else
					{

						char str[MAX_EXT_SIZE];
						cout << "The file " << event->name
						        << " was Created with WD " << event->wd << endl;
						//FindExt(event->name, str);
						if (!FindExt(event->name, str))
						{
							cout << "Ext : " << str << endl;
						}

					}
				}

				if (event->mask & IN_MODIFY)
				{
					if (event->mask & IN_ISDIR)
						cout << "The directory " << event->name
						        << " was modified.\n" << endl;
					else
						cout << "The file " << event->name
						        << " was modified with WD " << event->wd
						        << endl;
				}

				if (event->mask & IN_DELETE)
				{
					if (event->mask & IN_ISDIR)
						cout << "The directory " << event->name
						        << " was deleted.\n" << endl;
					else
						cout << "The file " << event->name
						        << " was deleted with WD " << event->wd << endl;
				}

				i += EVENT_SIZE + event->len;
			}
		}
	}

}

// This is the main program (i.e. the main thread)
int main(int argc, char *argv[])
{
	PR("Starting ...");
#ifdef TEST_MUTEX
	cout << endl << "Mutex locking (100 threads x 2 iterations)" << endl;
	// Clear the global counter.
	gCount = 0;

	// Start a bunch of child threads
	list<thread *> threadList;
	for(int i = 0; i < 100; ++ i)
	threadList.push_back(new thread(ThreadLock, 0));

	// Wait for the threads to finish
	list<thread *>::iterator it;
	for(it = threadList.begin(); it != threadList.end(); ++ it)
	{
		thread * t = *it;
		t->join();
		delete t;
	}
	`
	// Check the global count
	cout << " gCount = " << gCount << endl;
#endif

	int wd;
	int fd;
	struct stat directoryStatus;
	if (argc < 2)
	{
		cout << "Pass Folder Path as Second Argument." << endl;
		exit(0);
	}
	else
	{
		cout << argv[1] << endl;
		if (stat(argv[1], &directoryStatus)
		        == 0&& S_ISDIR(directoryStatus.st_mode))
		{
			cout << "Director found.\n" << endl;
		}
		else
		{
			cout << "No directory found.\n Directory Created.\n" << endl;
			createDirectory(argv[1], 0777);
		}
	}
	/* Initialize Inotify*/
	fd = inotify_init();
	if (fd < 0)
	{
		cerr << "Couldn't initialize inotify" << endl;
	}

	/* add watch to starting directory */
	wd = inotify_add_watch(fd, argv[1], IN_CREATE | IN_MODIFY | IN_DELETE);
	if (wd == -1)
	{
		cerr << "Couldn't add watch to " << argv[1] << endl;
	}
	else
	{
		cerr << "Watching:: " << argv[1] << endl;
	}

	// Start the child thread
	thread t(mainThread, (int *) &fd);
	t.join();

	/* Clean up*/
	inotify_rm_watch(fd, wd);
	close(fd);

	exit(0);

}

