#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/uio.h> //readv writev
#include <unistd.h>
#include <fcntl.h>
#include <map>

#define MAINCHANNEL "/tmp/mainfifo-1"
#define BUFF 10

int NClients = 0;
struct clientData {
    int recvFD;
    int sendFD;
};
std::map<pthread_t, struct clientData> cData;

void* handleClient(void *args){
    std::string readDir = "/tmp/r";
    std::string writeDir = "/tmp/w";
    int* pid = (int *)args;
    printf("thread: pid: %d\n", *pid);
    std::string rDir = readDir + std::to_string(*pid);
    std::string wDir = writeDir + std::to_string(*pid);
    int f = mkfifo(rDir.c_str(), 0666);
    perror("thread: mkfifo r");
    int f1 = mkfifo(wDir.c_str(), 0666);
    perror("thread: mkfifo w");
    
    struct clientData _data;
    _data.recvFD = open(rDir.c_str(), O_RDONLY);
    perror("thread: recvFD");
    _data.sendFD = open(wDir.c_str(), O_WRONLY);
    perror("thread: sendFD");
    cData[pthread_self()] = _data;
    struct pollfd fd[1];
    fd[0].fd = _data.recvFD;
    fd[0].events = POLLIN;
    do {

        int ret = poll(fd, 1, -1);
        if (ret == -1) {
            perror ("thread: poll");
            close(_data.recvFD);
            close(_data.sendFD);
            pthread_exit(0);
        }
        if (fd[0].revents & POLLIN){
            printf ("thread: stdin is readable: %d\n", ret);
           struct iovec iov[2];
            char buff1[8], buff2[8];
            iov[0].iov_base = buff1;
            iov[1].iov_base = buff2;
            iov[0].iov_len = sizeof(buff1);
            iov[1].iov_len = sizeof(buff2);
            readv(fd[0].fd, iov, 2);
            std::cout<<"thread: recv msg: "<<buff1<<buff2<<"\n";
            for(auto client: cData){
                if(client.first != pthread_self()){
                    writev(client.second.sendFD, iov, 2);
                }
            }
        } 
        else if(fd[0].revents & POLLHUP || fd[0].revents & (POLLERR | POLLNVAL)) {
            printf("thread: socket error\n");
            close(_data.recvFD);
            close(_data.sendFD);
            pthread_exit(0);
        }
    } while(1);
    close(_data.recvFD);
    close(_data.sendFD);
    printf("Exits\n");
    pthread_exit(0);

}

int main(){
    mkfifo(MAINCHANNEL, 0666);
    struct pollfd mainfd[1];
    mainfd[0].fd = open(MAINCHANNEL, O_RDONLY);
    mainfd[0].events = POLLIN;
    do {
        int ret = poll(mainfd, 1, -1);
        if (ret == -1) {
            perror ("poll");
            return 1;
        }
        if (mainfd[0].revents & POLLIN){
            int recv_pid;
            pthread_t thread;
            int r = read(mainfd[0].fd, &recv_pid, sizeof(int));
            printf("recv new client: %d\n", recv_pid);
            pthread_create(&thread, NULL, handleClient, (void *)&recv_pid);
            
        } else if(mainfd[0].revents == POLLHUP){
            printf("All fd connected has shutdown\n");
            break;
        }
        else if(mainfd[0].revents & (POLLERR | POLLNVAL)) {
            printf("socket error\n");
            break;
        }
    } while(1);
    close(mainfd[0].fd);
    return 0;
}