#include <bits/stdc++.h>
#include <sys/stat.h> // mkfifo
#include <unistd.h> // write
#include <signal.h> // sigaction
#include <fcntl.h> // O_*

using namespace std;

#define fifo "/tmp/fifo-sigaction"

void handler(int sig, siginfo_t* siginfo, void *context){
    cout<<"I process: "<<getpid()<<" recieved signal from process: "<<siginfo->si_pid<<".\n";
    raise(SIGINT);
}

int main(){
    // sigaction 
    struct sigaction _action;
    _action.sa_sigaction = &handler;
    _action.sa_flags = SA_SIGINFO;


	if (sigaction(SIGUSR1, &_action, NULL) < 0) {
		perror ("sigaction");
		return 1;
	}

    mkfifo(fifo, 0666);
    int fd = open(fifo, O_WRONLY);
    int pid = getpid();
    write(fd, &pid, sizeof(int));
    while(1){
        // keep waiting for other process to send a exit signal
        sleep(1);
    }
    return 0;
}