#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/poll.h>
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
    char* buff = (char *)args;
    printf("thread: uname: %s\n", buff);
    char* rDir = strcat((char *)readDir.c_str(), buff);
    char* wDir = strcat((char *)writeDir.c_str(), buff);
    printf("thread: fifos: %s, %s\n", rDir, wDir);
    int f = mkfifo(rDir, 0666);
    perror("thread: mkfifo rDir");
    int f1 = mkfifo(wDir, 0666);
    perror("thread: mkfifo wDir");
    
    struct clientData _data;
    _data.recvFD = open(rDir, O_RDONLY);
    perror("thread: recvFD");
    _data.sendFD = open(wDir, O_WRONLY);
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
            char buff[BUFF];
            read(fd[0].fd, buff, BUFF);
            buff[BUFF-1] = '\0';
            printf("thread: recv msg: %s\n", buff);
            for(auto client: cData){
                if(client.first != pthread_self()){
                    write(client.second.sendFD, buff, BUFF);
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
            char buff[4];
            pthread_t thread;
            int r = read(mainfd[0].fd, buff, 3);
            if(r != 3)continue;
            buff[3] = '\0';
            printf("recv new msg: %s\n", buff);
            pthread_create(&thread, NULL, handleClient, (void *)buff);
            
        } else if(mainfd[0].revents == POLLHUP){
            printf("All fd connected has shutdown\n");
            close(mainfd[0].fd);
            mainfd[0].fd = open(MAINCHANNEL, O_RDONLY);
        }
        else if(mainfd[0].revents & (POLLERR | POLLNVAL)) {
            printf("socket error\n");
            break;
        }
    } while(1);
    close(mainfd[0].fd);
    return 0;
}