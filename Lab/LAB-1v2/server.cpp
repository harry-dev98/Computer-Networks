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
#include <unistd.h>

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
int popen_fd, pipe_fd;

#define READ 0
#define WRITE 1

int POPENPGID=1000001, PIPEPGID = 1000002;


// assumed new process will just read so type always == "w"
// custom popen2 as per the requirement of situation
int popen2_ForFdAndPid(string cmd, int& newpid, int newPGId){
    int fd[2];
    pipe(fd);
    if((newpid = fork())==-1){
        perror("fork");
    }
    if(newpid == 0){
        close(fd[WRITE]);
        dup2(fd[READ], 0);

        // added new process A to POPENPGID
        setpgid(0, newPGId);
        char* const args[]= {NULL, }; 
        execvp(cmd.c_str(), args);
    }
    close(fd[READ]);
    return fd[WRITE];
}

void sigintHandler(int signum){
    cout<<"Total signals recieved: "<<sigCount<<"\n";
    signal(SIGINT, SIG_DFL);
    raise(SIGINT);
    exit(0);
}

void* sendDataViaPopen(void* args){
    cout<<"sending data to via POPEN\n";
    while(idx_start <= idx_end){
        sleep(1);

        // used custom popen to do setpgid on new process
        write(popen_fd, &buffer[idx_start++], sizeof(char));
        cout<<idx_start<<"send popen\n";
    }
    cout<<"no more data to send\n";
    pthread_exit(0);   
}

void* sendDataViaPipe(void* args){
    cout<<"sending data via PIPE\n";
    while(idx_start <= idx_end){
        sleep(1);

        // used named pipe 
        write(pipe_fd, &buffer[idx_start++], sizeof(char));
        cout<<idx_start<<"send pipe\n";
    }
    cout<<"no more data to send\n";
    pthread_exit(0);   
}

void* recvData(void* args){
    cout<<"recieving data from CD\n";


    // used named pipe
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


    // used killpg
    // used SIGSTOP
    // used SIGCONT

    killpg(POPENPGID, SIGSTOP);
    killpg(PIPEPGID, SIGCONT);
    
    pthread_create(&th, NULL, &sendDataViaPipe, NULL);
    sigCount++;
}

void handleSIGUSR2(int signum){
    cout<<"got interupt 2\n";
    if(sigCount!=0){
        pthread_cancel(th);
    }

    // used killpg
    // used SIGSTOP
    // used SIGCONT

    killpg(PIPEPGID, SIGSTOP);
    killpg(POPENPGID, SIGCONT);
    pthread_create(&th, NULL, &sendDataViaPopen, NULL);
    sigCount++;
}

int main(){
    signal(SIGINT, sigintHandler);
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

        // adding all the childs to PIPEPGID group
        setpgid(0, PIPEPGID);

    }

    if(pid > 0){
        signal(SIGUSR1, handleSIGUSR1);
        signal(SIGUSR2, handleSIGUSR2);
        int readerPid;
        popen_fd = popen2_ForFdAndPid(PROCESSA, readerPid, POPENPGID);
        pipe_fd = open(FROMS, O_WRONLY);


        // used killpg
            // used SIGSTOP
            // used SIGCONT
        killpg(POPENPGID, SIGSTOP);
        killpg(PIPEPGID, SIGSTOP);

        pthread_create(&th, NULL, &recvData, NULL);

    } else if(pid == 0){

        //used poll
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
    pthread_join(th, NULL);
    close(popen_fd);
    close(pipe_fd);
    cout<<"Total signals recieved: "<<sigCount<<"\n";
    return 0;
}