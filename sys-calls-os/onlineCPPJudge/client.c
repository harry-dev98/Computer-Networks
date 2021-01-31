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

std::string writeDir = "/tmp/w";

int main(){
    /*
        this section sends the code
    */
    printf("enter %d digit uname : ", 3);
    char uname[10];
    scanf("%s", uname);
    printf("enter fname : ");
    char fname[10];
    scanf("%s", fname);
    // FILE* f = fopen(fname, "r");
    // char fcontent[1000];
    // int idx=0;
    // char c;
    // while (fscanf(f , "%c" ,&c) == 1)
    // {
    //     fcontent[idx] = c;
    //     idx++;
    // }
    // fcontent[idx] = '\0';
    char data[20];
    sprintf(data, "%s%s", uname, fname);
    int _mainfd = open(MAINCHANNEL, O_WRONLY);
    write(_mainfd, data, 20);
    printf("sent a ack to server with uname: %s\n", uname);

    /*
        this section waits for judgement
    */
    char* wDir = strcat((char *)writeDir.c_str(), uname);
    mkfifo(wDir, 0666);
    perror("mkfifo r");
    int ret;
    struct pollfd fds[1];
    fds[0].fd = open(wDir, O_RDONLY);
    perror("open wDir");
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
            read(fds[0].fd, buff, 5);
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
    return 0;
}