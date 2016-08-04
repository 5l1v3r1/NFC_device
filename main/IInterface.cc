#include <iostream>
#include <string>

#include "IInterface.h"
//#include "mainIInterface.h"

using namespace std;

Observer::Observer()
{
	_InotifyInfo.fd = 0;
	_InotifyInfo.wd = 0;
	_InotifyInfo.path = DEFAULT_INOTIFY_PATH;
	cout << "Taking default path : " << _InotifyInfo.path << endl;
}

Observer::Observer(char **argv)
{
	_InotifyInfo.wd = 0;
	_InotifyInfo.path = argv[1];
	_InotifyInfo.fd = 0;
}

int Observer::initialize(void)
{
	struct stat directoryStatus;
	{
		cout << "Watch directory : " << _InotifyInfo.path << endl;
		if (stat(_InotifyInfo.path, &directoryStatus)
		        == 0 && S_ISDIR(directoryStatus.st_mode)) {
			cout << "Director found." << endl;
		}
		else {
			cout << "No directory found.\nCreating Directory" << endl;
			if(createDirectory(_InotifyInfo.path, 0777))
			{
				cerr << "Cannot able to create directory." << endl;
				return -1;
			}
		}
	}

	/*	create one instance of inotify*/
	if(_InotifyInfo.fd == 0)
	{
		_InotifyInfo.fd = inotify_init();
		if (_InotifyInfo.fd < 0) {
			cerr << "Couldn't initialize inotify" << endl;
			return -1;
		}
	}

	/*	add watch to starting directory*/
	if(_InotifyInfo.wd == 0)
	{
		_InotifyInfo.wd = inotify_add_watch(_InotifyInfo.fd, _InotifyInfo.path,
		IN_CREATE | IN_MODIFY | IN_DELETE | IN_OPEN | IN_MOVE | IN_CLOSE);
		if (_InotifyInfo.wd == -1) {
			cerr << _InotifyInfo.path << " : Couldn't add to watch" << endl;
			return -1;
		}
		else {
			cout << "Watching:: " << _InotifyInfo.path << endl;
		}
	}
	return 0;
}

int Observer::cleanup()
{
	/*	Clean up*/
	int ret = inotify_rm_watch(_InotifyInfo.fd, _InotifyInfo.wd);
	::close(_InotifyInfo.fd);
	return ret;
	
}
void Observer::Register(IInterface* p)
{
	medium.push_back(p);
}

// Will call Notify when change done
void Observer::Change(int p)
{
	_State = p;
	if(_State == created)
		Notify_created();
	else if(_State == modified)
		Notify_modified();
	else if(_State == deleted)
		Notify_deleted();
}

// Called hen event changed
// can check for some condition and then call Update funtions also.
void Observer::Notify_created()
{
	for(vector<IInterface*>::iterator it = medium.begin() ; 
		it != medium.end() ; ++it )
		(*it)->created(_State);
}
void Observer::Notify_modified()
{
	for(vector<IInterface*>::iterator it = medium.begin() ; 
		it != medium.end() ; ++it )
		(*it)->modified(_State);
}
void Observer::Notify_deleted()
{
	for(vector<IInterface*>::iterator it = medium.begin() ; 
		it != medium.end() ; ++it )
		(*it)->deleted(_State);
}

int Observer::CreateThread()
{
	/*	Start the child thread*/
	tthread::thread t(Observer::InotifyLoop, (void *) this);
	t.join();
	/*	Clean up*/
	inotify_rm_watch(_InotifyInfo.fd, _InotifyInfo.wd);
	::close(_InotifyInfo.fd);
}

void Observer::InotifyLoop(void *Observer_ptr)
{
	cout << "InotifyLoop() called" << endl;

	// wait in while loop to test thread 
	// while(1);

	int length = 0;
	int i = 0;
	char buffer[BUF_LEN];
	int ret = 0;
	int prevCheckSum = 0;

	Observer* arg = (Observer *) Observer_ptr;
	struct inotify_event *event = arg->_Event;
	inotifyFd InotifyInfo = (inotifyFd) (arg->_InotifyInfo);
	std::vector<IInterface *> medium = arg->medium;

	printf(" buffer : %s \n", InotifyInfo.path);

	/*	do it forever*/
	while (1)
	{
		i = 0;
		length = 0;
		memset(buffer, '\0', sizeof(buffer));

		length = read(InotifyInfo.fd, buffer, BUF_LEN);
		if (length < 0) {
			cerr << "read Event error" << endl;
		}

		cout << "Event Buffer Length : " << length << endl;
		while (i < length) {
			//struct inotify_event *event = (struct inotify_event *) &buffer[i];
			event = (struct inotify_event *) &buffer[i];
			if (event->len) {
				cout << "checking Event" << endl;
				if (event->mask & IN_CREATE) {
					cout << "IN create Event" << endl;
					if (event->mask & IN_ISDIR) {
						cout << "The directory " << event->name
						        << " was Created" << endl;
					}
					else {
						cout << "The file " << event->name
						        << " was Created with WD " << event->wd << endl;
					}
				}
				if (event->mask & IN_MODIFY) {
					cout << "IN modify Event" << endl;
					if (event->mask & IN_ISDIR) {
						cout << "The directory " << event->name
						        << " was modified" << endl;
					}
					else {
						cout << "The file " << event->name
						        << " was modified with WD " << event->wd
						        << endl;
						//ret = execute_app(event, InotifyInfo);
						arg->Notify_modified();
						if (ret == CHILD_SUCCESSFUL) {
							cout << "NDEF send : Done" << endl;
							break;
						}
						if (ret == CHILD_SIGNALED || ret == CHILD_UNSUCCESSFUL) {
							cout << "NDEF send : Fail" << endl;
							globalCount++;
							if (globalCount < 5) {
								cout << "\nSending again ..." << endl;
								continue;
							}
							else {
								cout << "Not Able to Send Data" << endl;
								cout << "Mobile Not Detected" << endl;
								globalCount = 0;
							}
						}
						if (ret == EXT_NOT_MATCH) {
							cout << "Extension not matched" << endl;
							break;
						}
						if (ret == MESSAGE_REPEATED) {
							cout << "Message repeated" << endl;
							break;
						}
					}
				}
				if (event->mask & IN_MOVE) {
					cout << "IN move Event" << endl;
					if (event->mask & IN_ISDIR) {
						cout << "The directory " << event->name
						        << " was Move to/from" << endl;
					}
					else {
						cout << "The file " << event->name
						        << " was Move to/from " << event->wd << endl;
					}
				}
			}

			i += EVENT_SIZE + event->len;
		}
	}
}


void Bluetooth::created(int i)
{
	cout << "Bluetooth() created" << endl;
	_State = i;
}
void Bluetooth::modified(int i)
{
	cout << "Bluetooth() modified" << endl;
	_State = i;
}
void Bluetooth::deleted(int i)
{
	cout << "Bluetooth() deleted" << endl;
	_State = i;
}



void Wifi::created(int i)
{
	cout << "Wifi() created" << endl;
	_State = i;
}
void Wifi::modified(int i)
{
	cout << "Wifi() modified" << endl;
	_State = i;
}
void Wifi::deleted(int i)
{
	cout << "Wifi() deleted" << endl;
	_State = i;
}
