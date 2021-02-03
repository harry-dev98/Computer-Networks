#include <bits/stdc++.h>
#include <signal.h> // kill
#include <sys/stat.h> //fifo
#include <fcntl.h> //O_*
#include <unistd.h> // read, write

using namespace std;

#define fifo "/tmp/fifo-sigaction"

int main(){
    mkfifo(fifo, 0666);
    int fd = open(fifo, O_RDONLY);
    int pid;
    read(fd, &pid, sizeof(int));
    kill(pid, SIGUSR1);
    cout<<"I, process: "<<getpid()<<" sent signal to process: "<<pid<<".\n";
    return 0;
}