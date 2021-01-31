#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <sys/poll.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define N 3
#define MAINCHANNEL "/tmp/mainfifo-1"
#define BUFF 10

std::string readDir = "/tmp/r";
std::string writeDir = "/tmp/w";

int main(){
    printf("enter %d digit uname : ", 3);
    char uname[3];
    scanf("%s", uname);
    int _mainfd = open(MAINCHANNEL, O_WRONLY);
    write(_mainfd, uname, 3);
    printf("sent a ack to server with uname\n");

    char* rDir = strcat((char *)readDir.c_str(), uname);
    char* wDir = strcat((char *)writeDir.c_str(), uname);
    int f = mkfifo(rDir, 0666);
    perror("mkfifo r");
    int f1 = mkfifo(wDir, 0666);
    perror("mkfifo w");
    
    printf("forked!\n");
    int pid = fork();

    
    int ret;
    if(pid == 0){
        struct pollfd fds[1];
        fds[0].fd = open(wDir, O_RDONLY);
        perror("open wDir0");
        fds[0].events = POLLIN;
        printf("child waits for POLLIN!\n");
        do{
            ret = poll(fds, 1, -1);
            if(ret < 0){
               break;
            }
            if (fds[0].revents & POLLIN){
                printf ("stdin is readable: %d\n", ret);
                char buff[BUFF];
                read(fds[0].fd, buff, BUFF);
                printf("%s\n", buff);
            } else if(fds[0].revents & POLLHUP){
                printf("All fd connected has shutdown\n");
                break;
            }
            else if(fds[0].revents & (POLLERR | POLLNVAL)) {
                printf("socket error\n");
                break;
            }
        } while(1);
        close(fds[0].fd);
    } else if(pid > 0){
        int sendFD;
        sendFD = open(rDir, O_WRONLY);
        perror("open rDir");
        printf("parent waits for message!\n");
        while(1){
            printf("enter message of length %d:\n", BUFF-1);
            char msg[BUFF];
            scanf("%s", msg);
            write(sendFD, msg, BUFF);
        }
        close(sendFD);
        wait(NULL);
    } else {
        return 0;
    }
    return 0;
}