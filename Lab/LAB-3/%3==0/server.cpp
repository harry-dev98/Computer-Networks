#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
using namespace std;

#define BUFF 10
#define MSG 1024

using namespace std;

struct msg {
    long mtype;
    char msg[MSG];
};

vector<pid_t> g1CPids, g2CPids;
vector<pthread_t> g1CThreads, g2CThreads;
pthread_t globalThread, g1GreetThread, g2GreetThread, g1ListenThread, g2ListenThread, statsThread;
int msgqid;
int infoG1Type=1, infoG2Type=2, msgG1Type=3, msgG2Type=4, globalType=5;

void handleClient(msg *buff){
    cout<<"pid recv: "<< buff->msg<<"\n";
    pid_t cPid = stoi(buff->msg);
    if(buff->mtype == infoG1Type){
        cout<<"added to g1cpids"<<cPid<<"\n";
        g1CPids.push_back(cPid);
    } else {
        g2CPids.push_back(cPid);
    }
}

void printStateOfQ(int msgqid){
    msqid_ds info;
    msgctl(msgqid, IPC_STAT, &info);
    cout<<"Last write time: "<<info.msg_stime<<" by process: "<<info.msg_lspid<<"\n";
    cout<<"Last read time: "<<info.msg_stime<<" by process: "<<info.msg_lrpid<<"\n";
    cout<<"Last update time: "<<info.msg_ctime<<"\n";
    cout<<"Total messages in queue are: "<<info.msg_qnum<<"\n";
    cout<<"clients in g1: ["<<g1CPids.size()<<"] => ";
    for(pid_t _: g1CPids)cout<<_<<" ";
    cout<<"\n";
    cout<<"clients in g2: ["<<g2CPids.size()<<"] => ";
    for(pid_t _: g2CPids)cout<<_<<" ";
    cout<<"\n";
}


void* printStats(void *arg){
    while(1){
        sleep(12);
        cout<<"stats of Q:\n";
        printStateOfQ(msgqid);
    }
    pthread_exit(0);
}

void handleExit(int sig, siginfo_t* siginfo, void *context){
    cout<<"IPC_RMID q\n";
    msgctl(msgqid, IPC_RMID, NULL);
    pthread_cancel(statsThread);
    signal(SIGINT,SIG_DFL);
    raise(SIGINT);
}

void* greetG1(void* arg){
    do {
        pthread_t thread;
        msg m;
        if(msgrcv(msgqid, &m, sizeof(m), infoG1Type, 0)==-1){
            cout<<"error with msgqid\n"; 
            break;
        }
        handleClient(&m);
        g1CThreads.push_back(thread);
    } while(1);
    cout<<"greetG1 exits\n";
    pthread_exit(0);
}

void* greetG2(void* arg){
    do {
        pthread_t thread;
        msg m;
        if(msgrcv(msgqid, &m, sizeof(m), infoG2Type, 0)==-1){
            cout<<"error with msgqid\n"; 
            break;
        }
        handleClient(&m);
        g2CThreads.push_back(thread);
    } while(1);
    cout<<"greetG2 exits\n";
    pthread_exit(0);
}

void splitMessage(string m, int &pid){
    int sz = m.size();
    for(int i=0; i<sz; i++){
        if(m[i] == ':'){
            pid = stoi(m.substr(0, i+1));
            cout<<"extracted pid: "<<pid<<"\n"; 
            break;
        }
    }
}

void sendMessageToPids(vector<pid_t> vec, string message, pid_t sender){
    
    msg m;
    message.copy(m.msg, message.size());
    m.msg[message.size()] = '\0';
    for(pid_t pid: vec){
            cout<<"sending msg to "<<pid<<"\n";

        if(pid != sender){
            m.mtype = pid;
            msgsnd(msgqid, &m, sizeof(m), 0);
        }
    }
}

void* listenG1(void* arg){
    do {
        msg m;
        msgrcv(msgqid, &m, sizeof(m), msgG1Type, 0);
        cout<<"message recvd in G1\n";
        for(pid_t _: g1CPids)cout<<_<<" - ";
        cout<<"\n";
        string message = string(m.msg);
        int pid;
        splitMessage(message, pid);
        for(int pids: g1CPids){
            cout<<pids<<" ";
        }cout<<endl;
        sendMessageToPids(g1CPids, message, pid);
    } while(1);
    cout<<"listenG1 exits\n";
    pthread_exit(0);

}


void* listenG2(void* arg){
    do {
        msg m;
            msgrcv(msgqid, &m, sizeof(m), msgG2Type, 0);
            cout<<"message recvd in G2\n";
        string message = string(m.msg);
        int pid;
        splitMessage(message, pid);
        sendMessageToPids(g2CPids, message, pid);
    } while(1);
    cout<<"listenG2 exits\n";
    pthread_exit(0);
}


void* listenGlobal(void* arg){
    do {
        msg m;
        msgrcv(msgqid, &m, sizeof(m), globalType, 0);
        string message = string(m.msg);
        int pid;
        splitMessage(message, pid);
        sendMessageToPids(g1CPids, message, pid);
        sendMessageToPids(g2CPids, message, pid);
    } while(1);
    cout<<"listenGlobal exits\n";
    pthread_exit(0);
}

int main(){
    struct sigaction _act;
    _act.sa_sigaction = &handleExit;
    _act.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &_act, NULL);
    system("touch ./server-client-lab3.txt");
    key_t key = ftok("./server-client-lab3.txt", 18);
    perror("key");

    msgqid = msgget(key, IPC_CREAT|0666);
    perror("get");
    pthread_create(&globalThread, NULL, listenGlobal, NULL);
    pthread_create(&g1ListenThread, NULL, listenG1, NULL);
    pthread_create(&g1ListenThread, NULL, listenG2, NULL);
    pthread_create(&statsThread, NULL, printStats, NULL);
    pthread_create(&g1GreetThread, NULL, greetG1, NULL);
    pthread_create(&g2GreetThread, NULL, greetG2, NULL);
    pthread_join(globalThread, NULL);
    pthread_join(g1ListenThread, NULL);
    pthread_join(g2ListenThread, NULL);
    pthread_join(statsThread, NULL);
    pthread_join(g1GreetThread, NULL);
    pthread_join(g2GreetThread, NULL);
    wait(NULL);
    msgctl(msgqid, IPC_RMID, NULL);
    exit(0);
}