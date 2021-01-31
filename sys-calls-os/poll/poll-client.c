#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define N 3
#define FIFOFILE "/tmp/FIFOFILE"
#define TIMEOUT 100
#define BUFF 10

int main(){
    pid_t pid;
    for(int i=0; i<3; i++){
        pid = fork();
        if(pid == 0){
            break;
        }
    }
    if(pid == 0){
        struct pollfd fds[1];
        int ret;

        fds[0].fd = open(FIFOFILE, O_WRONLY);
        fds[0].events = POLLOUT;
        printf("child to poll %d\n", getpid());
        ret = poll(fds, 1, TIMEOUT);
        if(ret < 0){
            
        } else if(ret == 0){
            printf("timeout\n");
        } else {
            printf("written\n");
            write(fds[0].fd, "123456789", BUFF);
        }
        printf("closed fd.\n");
        close(fds[0].fd);
    } else if(pid > 0){
        for(int i=0; i<N; i++){
            wait(NULL);
            printf("child %d finishes\n", i);
        }
    }
    return 0;
}