#include "Inotify.h"


using namespace std;
using namespace tthread;

#ifdef  __cplusplus
extern "C"
{
#endif

void InotifyLoop(void *arg)
{
	int length = 0,  i = 0;
	char buffer[BUF_LEN];
	int prevCheckSum = 0;

	inotifyFd InotifyInfo = *(inotifyFd *) arg;
	printf(" buffer : %s \n", InotifyInfo.path);

	/*	do it forever*/
	while (1)
	{
		i = 0;
		length = 0;
		buffer[0] = NULL;
		length = read(InotifyInfo.fd, buffer, BUF_LEN);
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
						cout << "The file " << event->name
								<< " was Created with WD " << event->wd << endl;
					}

				}
				if (event->mask & IN_MODIFY)
				{

					if (event->mask & IN_ISDIR)
					{
						cout << "The directory " << event->name
								<< " was modified.\n" << endl;
					}
					else
					{
						char str[MAX_EXT_SIZE];
						cout << "The file " << event->name
								<< " was modified with WD " << event->wd
								<< endl;
						if (!FindExt(event->name, str))
						{
							cout << "Ext : " << str << endl;
							if(strcmp("txt", str) == 0)
							{
								char *buffer[1];
								char str[128];
								int currentChSum = 0;
								strcpy(str,InotifyInfo.path);
								strcat(str,"/");
								strcat(str,event->name);

								ReadFile(str, buffer);

								currentChSum = CheckSum(buffer[0]);
								if(prevCheckSum != currentChSum)
								{
								prevCheckSum  = currentChSum;
								printf("modify done \n");
								break;
								}


//								ReadFile(str, buffer);
							}
						}

					}
				}
				if (event->mask & IN_MOVE)
				{
					if (event->mask & IN_ISDIR)
					{
						cout << "The directory " << event->name
								<< " was Move to/from.\n" << endl;
					}
					else
					{
						cout << "The file " << event->name
								<< "was Move to/from .." << event->wd << endl;
					}
				}
				if (event->mask & IN_CLOSE)	// will be called only when a file is open and close and will event will raise no. of times using editer but using .c program will raise this event once file operation is finished only once 
				{
					if (event->mask & IN_ISDIR)
					{
						cout << "The directory " << event->name
								<< " was CLOSE /from.\n" << endl;
					}
					else
					{
						cout << "The file " << event->name
								<< "was CLOSE" << event->wd << endl;
					///SendToNFC();
					}
				}
				if (event->mask & IN_DELETE)	// will be called only when a file is open and close and will event will raise no. of times using editer but using .c program will raise this event once file operation is finished only once
				{
					if (event->mask & IN_ISDIR)
					{
						cout << "The directory " << event->name
								<< " was IN_DELETE .\n" << endl;
					}
					else
					{
						cout << "The file " << event->name
								<< "was IN_DELETE" << event->wd << endl;
					///SendToNFC();
					}
				}

			}

			i += EVENT_SIZE + event->len;
		}
	}
}
#ifdef  __cplusplus
}
#endif

Inotify::Inotify()
{
	_InotifyInfo.fd  =  0;
	_InotifyInfo.wd  =  0;
	_InotifyInfo.path  =  NULL;

}

void Inotify::mainThread(char **argv)
{

	/*	 Initialize Inotify*/
	_InotifyInfo.path =  argv[1];
	printf("%s : \n", _InotifyInfo.path);
	_InotifyInfo.fd = inotify_init();
	if (_InotifyInfo.fd < 0)
	{
		cerr << "Couldn't initialize inotify" << endl;
	}

	/*	add watch to starting directory*/
	_InotifyInfo.wd = inotify_add_watch(_InotifyInfo.fd, argv[1],IN_CREATE | IN_MODIFY | IN_DELETE | IN_OPEN | IN_MOVE | IN_CLOSE);
	if (_InotifyInfo.wd == -1)
	{
		cerr << "Couldn't add watch to " << argv[1] << endl;
	}
	else
	{
		cerr << "Watching:: " << argv[1] << endl;
	}
	/*	Start the child thread*/
	thread t(InotifyLoop, (void *) &_InotifyInfo);
	t.join();
	/*	Clean up*/
	inotify_rm_watch(_InotifyInfo.fd, _InotifyInfo.wd);
	close(_InotifyInfo.fd);
}

