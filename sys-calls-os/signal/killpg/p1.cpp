// program creates a server to which each process
// gets registered with, server on its will 
// kill them all at a time, as server keep them saved in
// single process group

// imp to mention, server sets itself as group leader

#include <bits/stdc++.h>
#include <pthread.h> // threads
#include <unistd.h> // setpgid, etc
#include <fcntl.h> // O_*
#include <sys/poll.h> // poll
#include <sys/stat.h> // mkfifo

using namespace std;

#define fifo "/tmp/fifo-killpg"
#define MAXCLIENTS 4

int totalP = 0;
int pgid;
int pid = getpid();

void* thread_works(void* args){
    int *_pid = (int*)args;
    cout<<*_pid<<", "<<setpgid(*_pid, pgid);// == 0){
        perror("sas");
        cout<<"new process added\n";
        totalP++;
    // }
    sleep(3);
    // kill(*pid, SIGUSR1);
    pthread_exit(0);
}

void killMyClients(){
    killpg(SIGUSR1, pgid);
    cout<<"I am done, all process must act according with SIGUSR1 signum\nBYE!\n";
}

void handle(int signum){
    killMyClients();
    signal(SIGINT, SIG_DFL);
    raise(SIGINT);
}


int main(){
    setpgid(pid, pid);
    pgid = getpgid(0);
    cout<<"process: "<<pid<<", group: "<<pgid<<"\n";
    signal(SIGINT, handle);
    mkfifo(fifo, 0666);
    struct pollfd fd[1];
    fd[0].fd = open(fifo, O_RDONLY);
    fd[0].events = POLLIN;
    do {
        int ret = poll(fd, 1, -1);
        if (ret == -1) {
            perror ("poll");
            return 1;
        }
        if (fd[0].revents & POLLIN){
            int pid;
            read(fd[0].fd, &pid, sizeof(int));
            printf("recv new msg from process: %d\n", pid);
            pthread_t thread;
            pthread_create(&thread, NULL, thread_works, (void *)&pid);

        } else if(fd[0].revents == POLLHUP){
            printf("All fd connected has shutdown\n");
            break;
        }
        else if(fd[0].revents & (POLLERR | POLLNVAL)) {
            printf("socket error\n");
            break;
        }
    } while(totalP < MAXCLIENTS);
    close(fd[0].fd);
    killMyClients();   
    return 0;
}