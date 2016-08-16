#include "Observer.h"

// int Observer::CHILD_SUCCESSFUL = 0;
// int Observer::CHILD_UNSUCCESSFUL = 1;
// int Observer::CHILD_SIGNALED = 2;
// int Observer::EXT_NOT_MATCH = 3;
// int Observer::MESSAGE_REPEATED = 4;
// int Observer::OTHER = 9;
int Observer::globalCount = 0;

Observer::Observer() {
	_InotifyInfo.fd = 0;
	_InotifyInfo.wd = 0;
	_InotifyInfo.path = DEFAULT_INOTIFY_PATH;
	//memset (&_Event, 0 , sizeof(struct inotify_event));
	_Event = NULL;
	cout << "Taking default path : " << _InotifyInfo.path << endl;
}

Observer::Observer(char **argv) {
	_InotifyInfo.wd = 0;
	_InotifyInfo.path = argv[1];
	_InotifyInfo.fd = 0;
	//memset (&_Event, 0 , sizeof(struct inotify_event));
	_Event = NULL;
}

int Observer::initialize(void) {
	struct stat directoryStatus;
	{
		cout << "Watch directory : " << _InotifyInfo.path << endl;
		if (stat(_InotifyInfo.path, &directoryStatus)
		        == 0&& S_ISDIR(directoryStatus.st_mode)) {
			cout << "Director found." << endl;
		}
		else {
			cout << "No directory found.\nCreating Directory" << endl;
			if (createDirectory(_InotifyInfo.path, 0777)) {
				cerr << "Cannot able to create directory." << endl;
				return -1;
			}
		}
	}

	/*	create one instance of inotify*/
	if (_InotifyInfo.fd == 0) {
		_InotifyInfo.fd = inotify_init();
		if (_InotifyInfo.fd < 0) {
			cerr << "Couldn't initialize inotify" << endl;
			return -1;
		}
	}

	/*	add watch to starting directory*/
	if (_InotifyInfo.wd == 0) {
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

int Observer::cleanup() {
	/*	Clean up*/
	int ret = inotify_rm_watch(_InotifyInfo.fd, _InotifyInfo.wd);
	::close(_InotifyInfo.fd);
	return ret;
}

void Observer::subscribe(IInterface* p) {
	medium.push_back(p);
}

void Observer::unSubscribe(IInterface *p) {
	std::vector<IInterface *>::iterator it = medium.end();
	medium.erase(it);
}

// Will call Notify when change done
void Observer::change(int p) {
	int ret = 0;
	_State = p;
	if (_State == created)
		ret = notify_created();
	else if (_State == modified)
		ret = notify_modified();
	else if (_State == deleted)
		ret = notify_deleted();
	//return ret;
}

// Called hen event changed
// can check for some condition and then call Update funtions also.
int Observer::notify_created() {
	int ret = 0;
	for (vector<IInterface*>::iterator it = medium.begin(); it != medium.end();
	        ++it) {
		send_created_again: ret = (*it)->statusresponse(
		        (*it)->created(_Event, _InotifyInfo));
		if (ret == SEND_DATA_AGAIN) {
			goto send_created_again;
		}
		else if (ret == CHILD_UNSUCCESSFUL) {
			// Decide what to do if data send not successful
		}
	}
	return ret;
}

int Observer::notify_modified() {
	int ret = 0;
	for (vector<IInterface*>::iterator it = medium.begin(); it != medium.end();
	        ++it) {
		send_modified_again: ret = (*it)->statusresponse(
		        (*it)->modified(_Event, _InotifyInfo));
		if (ret == SEND_DATA_AGAIN) {
			goto send_modified_again;
		}
		else if (ret == CHILD_UNSUCCESSFUL) {
			// Decide what to do if data send not successful
		}
	}
	return ret;
}

int Observer::notify_deleted() {
	int ret = 0;
	for (vector<IInterface*>::iterator it = medium.begin(); it != medium.end();
	        ++it) {
		send_deleted_again: ret = (*it)->statusresponse(
		        (*it)->deleted(_Event, _InotifyInfo));
		if (ret == SEND_DATA_AGAIN) {
			goto send_deleted_again;
		}
		else if (ret == CHILD_UNSUCCESSFUL) {
			// Decide what to do if data send not successful
		}
	}
	return ret;
}

int Observer::createThread() {
	/*	Start the child thread*/
	tthread::thread t(Observer::InotifyLoop, (void *) this);
	t.join();
	/*	Clean up*/
	inotify_rm_watch(_InotifyInfo.fd, _InotifyInfo.wd);
	::close(_InotifyInfo.fd);
}

void Observer::InotifyLoop(void *Observer_ptr) {
	cout << "InotifyLoop() called" << endl;

	int length = 0;
	int i = 0;
	char *buffer = (char *) malloc(sizeof(char) * BUF_LEN);
	int ret = 0;
	int prevCheckSum = 0;

	Observer* arg = (Observer *) Observer_ptr;
	struct inotify_event *event;
	inotifyFd *InotifyInfo = &arg->_InotifyInfo;
	std::vector<IInterface *> medium = arg->medium;

	printf(" buffer : %s \n", InotifyInfo->path);

	/*	do it forever*/
	while (1) {
		i = 0;
		length = 0;
		memset(buffer, '\0', sizeof(buffer));

		length = read(InotifyInfo->fd, buffer, BUF_LEN);
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
					cout << "IN : create Event" << endl;
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
					cout << "IN : modify Event" << endl;
					if (event->mask & IN_ISDIR) {
						cout << "The directory " << event->name
						        << " was modified" << endl;
					}
					else {
						cout << "The file " << event->name
						        << " was modified with WD " << event->wd
						        << endl;

						arg->_Event = event;
						arg->change(modified);

						/*if (ret == CHILD_SUCCESSFUL) {
						 cout << "NDEF send : Done" << endl;
						 //medium[resLoop]->resetStatus();
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
						 }*/
						//}
					}
				}

				if (event->mask & IN_MOVE) {
					cout << "IN : move Event" << endl;
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
