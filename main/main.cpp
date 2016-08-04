// This is the main program (i.e. the main thread)
#include "IInterface.h"
#include <iostream>

using namespace std;

static Observer *monitor = NULL;

void catch_signal(int signalNumber)
{
	cerr << "Caught exception" << endl;
	/*
	moniter->cleanup();
	delete moniter;
	moniter = NULL;
	*/
}

int main(int argc, char *argv[])
{
	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = catch_signal;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	if (argc < 2) {
		cout << DEFAULT_INOTIFY_PATH << "Taking default dictory as path" << endl;
		monitor = new Observer();
	}
	else {
		monitor = new Observer(argv);
	}

	try
	{
		int ret = 0;
		Wifi dev;
		monitor->Register(&dev);
		monitor->initialize();
		if(ret == 0)
			monitor->CreateThread();
	}
	catch(...)
	{
		cerr << "Caught unexpected exception: " << endl;
		monitor->cleanup();
		delete monitor;
		monitor = NULL;
	}

	return 0;
}
