#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <sys/poll.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/uio.h> // readv writev

#define N 3
#define MAINCHANNEL "/tmp/mainfifo-1"
#define BUFF 8

std::string readDir = "/tmp/r";
std::string writeDir = "/tmp/w";

int main(){
    int mypid = getpid();
    int _mainfd = open(MAINCHANNEL, O_WRONLY);
    write(_mainfd, &mypid, sizeof(int));
    printf("sent a ack to server with uname\n");

    std::string rDir = readDir + std::to_string(mypid);
    std::string wDir = writeDir + std::to_string(mypid);
    int f = mkfifo(rDir.c_str(), 0666);
    perror("mkfifo r");
    int f1 = mkfifo(wDir.c_str(), 0666);
    perror("mkfifo w");
    
    printf("forked!\n");
    int pid = fork();

    int ret;
    if(pid == 0){
        struct pollfd fds[1];
        fds[0].fd = open(wDir.c_str(), O_RDONLY);
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
                struct iovec iov[2];
                char buff1[8], buff2[8];
                iov[0].iov_base = buff1;
                iov[1].iov_base = buff2;
                iov[0].iov_len = sizeof(buff1);
                iov[1].iov_len = sizeof(buff2);
                readv(fds[0].fd, iov, 2);
                std::cout<<buff1<<buff2<<"\n";
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
        sendFD = open(rDir.c_str(), O_WRONLY);
        perror("open rDir");
        while(1){
            struct iovec iov[2];
            char buff1[BUFF], buff2[BUFF];
            iov[0].iov_base = buff1;
            iov[1].iov_base = buff2;
            iov[0].iov_len = sizeof(buff1);
            iov[1].iov_len = sizeof(buff2);
            
            // readv(STDIN_FILENO, iov, 2); // problem with it is it reads all characters
                                            // even newline or space char
            std::cin>>buff1>>buff2;
            writev(sendFD, iov, 2);
        }
        close(sendFD);
        wait(NULL);
    } else {
        return 0;
    }
    return 0;
}