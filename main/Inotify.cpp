#include "Inotify.h"

using namespace std;
using namespace tthread;

#ifdef  __cplusplus
extern "C"
{
#endif

void InotifyLoop(void *arg)
{

	int length, i = 0;
	char buffer[BUF_LEN];
	int ret = 0;
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
						cout << "The directory " << event->name << " was Created.\n" << endl;
					}
					else
					{
						cout << "The file " << event->name << " was Created with WD " << event->wd << endl;
					}

				}
				if (event->mask & IN_MODIFY)
				{
					if (event->mask & IN_ISDIR)
					{
						cout << "The directory " << event->name << " was modified.\n" << endl;
					}
					else
					{
						char str[MAX_EXT_SIZE];
						if (!FindExt(event->name, str))
						{
							if (strcmp("txt", str) == 0)
							{
								char *buffer[1];
								char str[128] =
								{ '\0' };
								int currentChSum = 0;
								strcpy(str, InotifyInfo.path);
								strcat(str, "/");
								strcat(str, event->name);
///////////////////////////////////////////////////////////////////
								// convert message to ndef format.
								ReadFile(str, buffer);
								currentChSum = CheckSum(buffer[0]);
								if (prevCheckSum != currentChSum)
								{
									prevCheckSum = currentChSum;

									pid_t pid = 0;
									int status = 0;

//////////      first server will check its is having an handshake
									pid = fork();
									if (pid == 0)
									{
										chdir("../MyLib/libllcp");
										execlp("./examples/snep-server/snep-server", "./examples/snep-server/snep-server", "-o", "n", NULL);
										perror("unable to load ./snep-server from libllcp");
										prevCheckSum = 0;
									}
									ret = waitpid(pid, &status, 0);
									if (ret == -1)
									{
										prevCheckSum = 0;
										perror(" wait fails snep-server");
									}
									else if (!WIFEXITED(status))
									{
										prevCheckSum = 0;
										printf("snep-encode terminated abnormally");
									}
									else if (WEXITSTATUS(status) != 0)
									{
										prevCheckSum = 0;
										printf("snep-encode failed");
									}

									if (prevCheckSum != 0)
									{

										status = 0;
										pid = 0;
										pid = fork(); //calling snep-encode which will convert new record to ndef format
										if (pid == 0)
										{
											chdir("../main");
											ret = system("export LLD_LIBRARY_PATH=\"../extras/libndef/libndef\"");
											if (ret == -1)
											{
												perror("unable to load export LD_LIBRARY_PATH=/home/ratnesh/programs/project/NFC_device/extras/libndef");
											}
											execlp("./snep-encode", "./snep-encode", str, "en-US", NULL);
											perror("unable to load ./snep-encode");
										}

										ret = waitpid(pid, &status, 0);
										if (ret == -1)
										{
											prevCheckSum = 0;
											perror("waitpid fails");
										}
										else if (!WIFEXITED(status))
										{
											prevCheckSum = 0;
											printf("snep-encode terminated abnormally");
										}
										else if (WEXITSTATUS(status) != 0)
										{
											prevCheckSum = 0;
											printf("snep-encode failed");
										}

										// send it to nfc through libllcp
										status = 0;
										pid = 0;
										pid = fork();
										if (pid == 0)
										{
											chdir("../MyLib/libllcp");
											execlp("./examples/snep-client/snep-client", "./examples/snep-client/snep-client", "../../main/receipt_nfc", NULL);
											perror("unable to load ./snep-client from libllcp");
											prevCheckSum = 0;
										}
										pid = waitpid(pid, &status, 0);
										if (pid == -1)
										{
											prevCheckSum = 0;
											perror("");
										}
										else if (!WIFEXITED(status))
										{
											printf("snep-encode terminated abnormally");
											prevCheckSum = 0;
										}
										else if (WEXITSTATUS(status) != 0)
										{
											printf("snep-encode failed");
											prevCheckSum = 0;
										}
										cout << "The file " << event->name << " was modified with WD " << event->wd << endl;
									}
									break;
								}
							}
						}
					}
				}
				if (event->mask & IN_MOVE)
				{
					if (event->mask & IN_ISDIR)
					{
						cout << "The directory " << event->name << " was Move to/from.\n" << endl;
					}
					else
					{
						cout << "The file " << event->name << " was Move to/from .." << event->wd << endl;
						char str[MAX_EXT_SIZE];
						if (!FindExt(event->name, str))
						{
							if (strcmp("txt", str) == 0)
							{
								char *buffer[1];
								char str[128] =
								{ '\0' };
								strcpy(str, InotifyInfo.path);
								strcat(str, "/");
								strcat(str, event->name);
								cout << "Process Ext : " << str << endl;
///////////////////////////////////////////////////////////////////
								// convert message to ndef format.
								pid_t pid;
								int status;
#if 0
								pid = fork();
								if (pid == 0)
								{
									chdir("../main");
									ret = system("export LLD_LIBRARY_PATH=\"../extras/libndef/libndef\"");
									if (ret == -1)
									{
										perror("unable to load export LD_LIBRARY_PATH=/home/ratnesh/programs/project/NFC_device/extras/libndef");
									}
									ret = system("./run.sh");
									if (ret == -1)
									{
										perror("unable to load ./run.sh");
									}
									execlp("./snep-encode", "./snep-encode", str, "en-US", NULL);
									perror("unable to load ./snep-encode");
									exit(0);
								}

								pid = waitpid(pid, &status, 0);

								if (pid == -1)
								{
									perror("");
									exit(2);
								}

								if (!WIFEXITED(status))
								{
									printf("snep-encode terminated abnormally");
									exit(3);
								}
								if (WEXITSTATUS(status) != 0)
								{
									printf("snep-encode failed");
									exit(3);
								}

								// send it to nfc through libllcp
								pid = fork();
								if (pid == 0)
								{
									chdir("../MyLib/libllcp");
									execlp("./examples/snep-client/snep-client", "./examples/snep-client/snep-client", "../../main/receipt_nfc", NULL);
									perror("unable to load ./snep-client from libllcp");
									exit(0);
								}
								pid = waitpid(pid, &status, 0);
								if (pid == -1)
								{
									perror("");
									exit(2);
								}
								if (!WIFEXITED(status))
								{
									printf("snep-encode terminated abnormally");
									exit(3);
								}
								if (WEXITSTATUS(status) != 0)
								{
									printf("snep-client failed");
									exit(3);
								}

///////////////////////////////////////////////////////////////////
//								ReadFile(str, buffer);
#endif
							}
						}
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
	_InotifyInfo.fd = 0;
	_InotifyInfo.wd = 0;
	_InotifyInfo.path = NULL;

}

void Inotify::mainThread(char **argv)
{

	/*	 Initialize Inotify*/
	_InotifyInfo.path = argv[1];
	printf("%s : \n", _InotifyInfo.path);
	_InotifyInfo.fd = inotify_init();
	if (_InotifyInfo.fd < 0)
	{
		cerr << "Couldn't initialize inotify" << endl;
	}

	/*	add watch to starting directory*/
	_InotifyInfo.wd = inotify_add_watch(_InotifyInfo.fd, argv[1], IN_CREATE | IN_MODIFY | IN_DELETE | IN_OPEN | IN_MOVE | IN_CLOSE);
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

