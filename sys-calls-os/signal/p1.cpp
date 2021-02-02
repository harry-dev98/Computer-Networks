#include <bits/stdc++.h>
#include <stdio.h>
#include <signal.h> //signal, kill
#include <sys/stat.h> //mkfifo
#include <sys/types.h> //types
#include <fcntl.h> //O_*
#include <unistd.h> // getpid
#include <pthread.h> //pthreads
#include <sys/ipc.h> //ftok
#include <sys/shm.h> //shmat, shget, etc

using namespace std;

string fifoP1 = "/tmp/p1";
string fifoP2 = "/tmp/p2";
string shmX = "/tmp/shmX";
string shmY = "/tmp/shmY";

int pid_other;
void handler(int signum){
    key_t ftok1 = ftok(shmX.c_str(), 18);
    int shmXId = shmget(ftok1, sizeof(int), 0666|IPC_CREAT);
    int *x = (int *) shmat(shmXId,NULL, 0);
    cout<<"Some process with pid: "<<pid_other<<" asked me to process: "<<*x<<".\n";

    key_t ftok2 = ftok(shmY.c_str(), 18);
    int shmYId = shmget(ftok2, sizeof(int), 0666|IPC_CREAT);
    int *y = (int *) shmat(shmYId, NULL, 0);
    *y = (*x) * (*x);
    shmdt (x);
    shmdt (y);
    shmctl (shmXId, IPC_RMID, 0);
    shmctl (shmYId, IPC_RMID, 0);
    kill(pid_other, SIGINT);
    cout<<"Writing the processed info to new shared memory y and leting the process know it.\n";
}

void* write(void *args){
    int wFd = open(fifoP1.c_str(), O_WRONLY);
    int pid = getpid();
    //writing my pid to p1
    write(wFd, &pid, 1024);
    close(wFd);
    pthread_exit(0);
}

void* read(void *args){
    int rFd = open(fifoP2.c_str(), O_RDONLY);
    //reading p2's pid;
    read(rFd, &pid_other, 1024);
    close(rFd);
    pthread_exit(0);
}

int endVar = 1;
// void exit(int signum){
//     raise(SIGABRT);
//     // endVar = 0;
// }

int main(){
    signal(SIGUSR1, handler);
    // signal(SIGUSR2, exit);
    mkfifo(fifoP1.c_str(), 0666);
    mkfifo(fifoP2.c_str(), 0666);
    pthread_t th1, th2;
    pthread_create(&th1, NULL, &write, (void*) NULL);
    pthread_create(&th2, NULL, &read, (void*) NULL);
    void* s1, *s2;
    pthread_join(th1, &s1);
    pthread_join(th2, &s2);
    cout<<"waiting for action\n";
    while(endVar){
        //ends only when other process talks to it once
    }
    cout<<"now exiting as other process asked me.\n";
    return 0;
}