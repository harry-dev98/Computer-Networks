#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/poll.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

using namespace std;
#define SCH 1
#define FROMS "/tmp/from-s"
#define FROMC "/tmp/from-c"
#define SIG "/tmp/signalTod"

#define PROCESSA "./A"

pthread_t th;
int sigCount = 0;

char buffer[1024];
int idx_end = 0;
int idx_start = 0;


void* sendDataViaPopen(void* args){
    cout<<"sending data to via POPEN\n";
    FILE* writer = popen(PROCESSA, "w");
    int _fd = fileno(writer);
    perror("popen");
    while(idx_start <= idx_end){
        sleep(1);
        write(_fd, &buffer[idx_start++], sizeof(char));
        cout<<idx_start<<"send popen\n";
    }
    cout<<"no more data to send\n";
    close(_fd);
    pthread_exit(0);   
}

void* sendDataViaPipe(void* args){
    cout<<"sending data via PIPE\n";
    int fd = open(FROMS, O_WRONLY);
    while(idx_start <= idx_end){
        sleep(1);
        write(fd, &buffer[idx_start++], sizeof(char));
        cout<<idx_start<<"send pipe\n";
    }
    cout<<"no more data to send\n";
    close(fd);
    pthread_exit(0);   
}

void* recvData(void* args){
    cout<<"recieving data from CD\n";
    int fd = open(FROMC, O_RDONLY);
    perror("recvData");
    while(1){
        char ch;
        sleep(1);
        if(!read(fd, &ch, sizeof(char)))break;
        cout<<idx_end<<"recieved from client: "<<ch<<"\n";
        buffer[idx_end++] = ch;
    }
    pthread_exit(0);
}

void handleSIGUSR1(int signum){
    cout<<"got interupt 1\n";
    if(sigCount!=0){
        pthread_cancel(th);
    }
    pthread_create(&th, NULL, &sendDataViaPipe, NULL);
    sigCount++;
}

void handleSIGUSR2(int signum){
    cout<<"got interupt 2\n";
    if(sigCount!=0){
        pthread_cancel(th);
    }
    pthread_create(&th, NULL, &sendDataViaPopen, NULL);
    sigCount++;
}


int main(){
    mkfifo(FROMS, 0666);
    perror("froms");
    mkfifo(FROMC, 0666);
    perror("fromc");
    mkfifo(SIG, 0666);
    perror("sig");
    int _fd = open(SIG, O_WRONLY);
    int _pid = getpid();
    write(_fd, &_pid, sizeof(int));
    close(_fd);

    cout<<"D knows my pid now!!\n";
    int pid;
    for(int i=0; i<SCH; i++){
        pid = fork();
        if(pid == 0)break;
    }

    if(pid != 0){
        signal(SIGUSR1, handleSIGUSR1);
        signal(SIGUSR2, handleSIGUSR2);
        pthread_create(&th, NULL, &recvData, NULL);

    } else if(pid == 0){
        pollfd fd[0];
        fd[0].fd = open(FROMS, O_RDONLY);
        fd[0].events = POLLIN;
        char ch;
        do {
            int ret = poll(fd, 1, -1);
            if(fd[0].revents & POLLIN){
                read(fd[0].fd, &ch, sizeof(char));
                cout<<"Child Process: "<<ch<<"\n";
            } else if(fd[0].revents & POLLHUP||fd[0].revents & POLLNVAL){
                break;
            }
        } while(1);
        close(fd[0].fd);
        return 0;
    }
    for(int i=0; i<SCH; i++){
        cout<<"waiting for child "<<i<<".\n";
        wait(NULL);
    }
    cout<<"Total signals recieved: "<<sigCount<<"\n";
    return 0;
}