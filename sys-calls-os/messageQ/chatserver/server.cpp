#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>

#define BUFF 10
#define MSG 10

using namespace std;

pthread_t stats;
struct msg {
    long mtype;
    char msg[MSG];
};

std::set<int> cData;
int msgqid1, msgqid2, msgqid3;

void* handleClient(void *args){
    msg* buff = (msg *)args;
    printf("thread: pid recv: %ld\n", buff->mtype);
    cData.insert(buff->mtype);
    do {
        msg m;
        std::cout<<"waiting for "<<buff->mtype<<"\n";
        msgrcv(msgqid1, &m, sizeof(m), buff->mtype, MSG_NOERROR);
        perror("msgrcv");
        if(m.mtype == buff->mtype)std::cout<<m.msg<<" eq\n";
        // std::cout<<"recieved from: "<<m.mtype<<"\n";
        for(auto cD: cData){
            msg tmp = m;
            if(m.mtype != cD){
                tmp.mtype = cD;
                std::cout<<"to process: "<<cD<<"\n";
                msgsnd(msgqid3, &tmp, sizeof(tmp), 0);
                perror("msgsend");
            }
        }
    } while(1);
    printf("Exits\n");
    pthread_exit(0);

}

void printStateOfQ(int msgqid){
    msqid_ds info;
    msgctl(msgqid, IPC_STAT, &info);
    cout<<"Last write time: "<<info.msg_stime<<" by process: "<<info.msg_lspid<<"\n";
    cout<<"Last read time: "<<info.msg_stime<<" by process: "<<info.msg_lrpid<<"\n";
    cout<<"Last update time: "<<info.msg_ctime<<"\n";
    cout<<"Total messages in queue are: "<<info.msg_qnum<<"\n";
}


void* printStats(void *arg){
    while(1){
        cout<<"stats if Q1:\n";
        printStateOfQ(msgqid1);
        cout<<"stats if Q2:\n";
        printStateOfQ(msgqid2);
        cout<<"stats if Q3:\n";
        printStateOfQ(msgqid3);
        sleep(10);
    }
}

void handleExit(int sig, siginfo_t* siginfo, void *context){
    msgctl(msgqid1, IPC_RMID, NULL);
    msgctl(msgqid2, IPC_RMID, NULL);
    pthread_cancel(stats);
    signal(SIGINT,SIG_DFL);
    raise(SIGINT);
}

int main(){
    struct sigaction _act;
    _act.sa_sigaction = &handleExit;
    _act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &_act, NULL);
    system("touch ./server-client1.txt");
    system("touch ./server-client2.txt");
    system("touch ./server-client3.txt");
    key_t key1 = ftok("./server-client1.txt", 1);
    key_t key2 = ftok("./server-client2.txt", 1);
    key_t key3 = ftok("./server-client3.txt", 1);
    msgqid1 = msgget(key1, IPC_CREAT|0666);
    msgqid2 = msgget(key2, IPC_CREAT|0666);
    msgqid3 = msgget(key3, IPC_CREAT|0666);

    pthread_create(&stats, NULL, printStats, NULL);
    do {
        pthread_t thread;
        msg m;
        msgrcv(msgqid2, &m, sizeof(m), 0, 0);
        pthread_create(&thread, NULL, handleClient, (void *)&m);
    } while(1);
    msgctl(msgqid1, IPC_RMID, NULL);
    msgctl(msgqid2, IPC_RMID, NULL);
    return 0;
}