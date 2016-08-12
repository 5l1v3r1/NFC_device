#include <iostream>
#include <string>

#include "IInterface.h"
#include "mainIInterface.h"

using namespace std;

int IInterface::statusresponse(int status) {
	return OTHER;
}

int Bluetooth::created(struct inotify_event *Event, inotifyFd InotifyInfo) {
	cout << "Bluetooth() created" << endl;
}

int Bluetooth::modified(struct inotify_event *Event, inotifyFd InotifyInfo) {
	cout << "Bluetooth() modified" << endl;
}

int Bluetooth::deleted(struct inotify_event *Event, inotifyFd InotifyInfo) {
	cout << "Bluetooth() deleted" << endl;
}

int Bluetooth::statusresponse(int status) {
	return OTHER;
}

int Wifi::created(struct inotify_event *Event, inotifyFd InotifyInfo) {
	cout << "Wifi() created" << endl;
	//_State = i;
}

int Wifi::modified(struct inotify_event *Event, inotifyFd inotifyInfo) {
	cout << "Wifi() modified" << endl;
	cout << "Event " << Event->name << endl;

#if defined(__ARM__) && not defined(__x86__)

	struct inotify_event *event = Event;
	inotifyFd InotifyInfo = inotifyInfo;

	int ret = 0;
	char str[MAX_EXT_SIZE];
	int prevCheckSum = 0;
	if (!FindExt(event->name, str)) {
		if (strcmp("txt", str) == 0) {
			//cout << "Extention matched" << endl;
			char *buffer[1];
			char str[128] = { '\0' };
			int currentChSum = 0;
			strcpy(str, InotifyInfo.path);
			strcat(str, "/");
			strcat(str, event->name);
			ReadFile(str, buffer);
			//cout << "Process Ext : " << str << endl;

			// convert message to ndef format.
			currentChSum = CheckSum(buffer[0]);
			// check if message not repeated
			if (prevCheckSum != currentChSum) {
				prevCheckSum = currentChSum;
				pid_t pid, wpid;
				int status;
				const char *env[] = {
				        "LD_LIBRARY_PATH=../extra/libndef/libndef", NULL };
				pid = fork();
				if (pid == 0) {
					cout << "Child run.sh started" << endl;
					if (chdir("../main")) {
						perror("unable to change directory : \"../main\"");
						exit(0);
					}

					ret = system("./run.sh");
					if (ret == -1) {
						perror("unable to load ./run.sh");
					}
					//execlp("./snep-encode", "./snep-encode", str, "en-US", NULL);
					execle("./snep-encode", "./snep-encode", str, "en-US", NULL,
					        env);
					perror("unable to load ./snep-encode");
					exit(-1);
				}
				wpid = waitpid(pid, &status, 0);
				if (wpid == -1) {
					perror("waitpid fail for run.sh and snep-encode process.");
					exit(2);
				}

				if (!WIFEXITED(status)) {
					perror("snep-encode terminated abnormally");
					exit(3);
				}
				if (WEXITSTATUS(status) != 0) {
					perror("snep-encode failed");
					exit(3);
				}

				// send it to nfc through libllcp
				int timeout = 5;
				pid = fork();
				if (pid == 0) {
					cout << "Child snep-client started timeout " << timeout
					        << " Second" << endl;
					if (chdir("../MyLib/libllcp")) {
						perror(
						        "unable to change directory : .\"./MyLib/libllcp\"");
						exit(3);
					}
					execlp("./examples/snep-client/snep-client",
					        "./examples/snep-client/snep-client",
					        "../../main/receipt_nfc", NULL);
					perror("unable to load ./snep-client from libllcp");
					exit(0);
				}

				printf("Child PID : %d\n", pid);
				int waittime = 0;
				status = 0;
				do {
					wpid = waitpid(pid, &status, WNOHANG);
					if (wpid == 0) {
						if (waittime < timeout) {
							printf("Parent waiting %d second(s).\n", waittime);
							sleep(1);
							waittime++;
						}
						else {
							printf("Killing child process : %d\n", pid);
							errno = kill(pid, SIGKILL);
							if (errno == ESRCH) {
								printf("child process already killed\n");
							}
							if (errno == EPERM) {
								printf("child process permission error\n");
							}
							wpid = waitpid(pid, &status, WSTOPPED);
							if (wpid == pid) {
								printf("Child process : %d resource cleaned\n",
								        wpid);
							}
							break;
						}
					}
				} while (wpid == 0 && waittime <= timeout);

				if (WIFEXITED(status)) {
					//printf("Child exited, status = %d\n", WEXITSTATUS(status));
					// In fail it exit with : 1 (WEXITSTATUS(status))
					// In success it exit with : 0   (WEXITSTATUS(status))
					if (WEXITSTATUS(status) == 1) {
						printf("Child work Unsuccessful\n");
						// NDEF message not transmitted but program came back normally.
						//setStatus(CHILD_UNSUCCESSFUL);
						return CHILD_UNSUCCESSFUL;
					}
					else if (WEXITSTATUS(status) == 0) {
						printf("Child work Done\n");
						// Child transmitted NDEF successfully.
						//setStatus(CHILD_SUCCESSFUL);
						return CHILD_SUCCESSFUL;
					}
				}
				else if (WIFSIGNALED(status)) {
					printf("Child %d was signaled with a status of: %d \n", pid,
					        WTERMSIG(status));
					//setStatus(CHILD_SIGNALED);
					return CHILD_SIGNALED;
				}
			}
			//setStatus(MESSAGE_REPEATED);
			return MESSAGE_REPEATED;
		}
		//setStatus(EXT_NOT_MATCH);
		return EXT_NOT_MATCH;
	}
#endif // macro over defined(__ARM__) && not defined(__x86__)
	//setStatus(OTHER);
	return OTHER;
}

int Wifi::deleted(struct inotify_event *Event, inotifyFd InotifyInfo) {
	cout << "Wifi() deleted" << endl;
}

//if return CHILD_SUCCESSFUL no neet to send data again
//if return CHILD_UNSUCCESSFUL then need to send data again
int Wifi::statusresponse(int status) {
	static int globalCount = 0;
	int ret = status;

	if (ret == CHILD_SUCCESSFUL) {
		cout << "NDEF send : Done" << endl;
		//break;
	}
	if (ret == CHILD_SIGNALED || ret == CHILD_UNSUCCESSFUL) {
		cout << "NDEF send : Fail" << endl;
		globalCount++;
		if (globalCount < 5) {
			cout << "\nSending again ..." << endl;
			//continue;
			return SEND_DATA_AGAIN;
		}
		else {
			cout << "Not Able to Send Data" << endl;
			cout << "Mobile Not Detected" << endl;
			globalCount = 0;
		}
	}
	if (ret == EXT_NOT_MATCH) {
		cout << "Extension not matched" << endl;
		//break;
	}
	if (ret == MESSAGE_REPEATED) {
		cout << "Message repeated" << endl;
		//break;
	}
	return CHILD_SUCCESSFUL;
}
