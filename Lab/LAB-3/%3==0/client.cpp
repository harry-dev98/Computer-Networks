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
using namespace std;

#define MAINCHANNEL "/tmp/mainfifo-1"
#define BUFF 10
#define MSG 1024

struct msg {
    long mtype;
    char msg[MSG];
};

std::set<int> cData;
int msgqid, pid;
string pid_str;

int infoType, msgType, recvType, globalType = 4;
void* handleSendMsg(void* arg){
    msg m;
    while(1){
        printf("Please enter message\n");
        string _msg;
        cin>>_msg;
        m.mtype = msgType;
        _msg = pid_str + ":" + _msg;
        _msg.copy(m.msg, _msg.size());
        m.msg[_msg.size()] = '\0';
        msgsnd(msgqid, &m, sizeof(m), 0);
        perror("msgsnd");
    }
    printf("Exits\n");
    pthread_exit(0);
}

void* handleRecvMsg(void* arg){
    msg m;
    while(1){
        std::cout<<"waiting for some msg\n";
        msgrcv(msgqid, &m, sizeof(m), recvType, 0);
        perror("msgrcv");
        std::cout<<"msg :"<<m.msg<<"\n";
    }
    printf("Exits\n");
    pthread_exit(0);
}

int main(){
    // sending my pid
    pid = getpid();
    pid_str = to_string(pid);
    cout<<"Type 1 if you are in Group1 else 2\n";
    int n;
    cin>>n;
    if(n == 1){
        infoType = 0;
        msgType = 2;
    } else if(n == 2) {
        infoType = 1;
        msgType = 3;
    } else {
        return 0;
    }
    recvType = pid;

    system("touch ./server-client-lab3.txt");
    key_t key = ftok("./server-client-lab3.txt", 1);
    msgqid = msgget(key, IPC_CREAT|0666);

    msg m;
    m.mtype=infoType; 
    pid_str.copy(m.msg, pid_str.size());
    m.msg[pid_str.size()] = '\0';
    msgsnd(msgqid, &m, sizeof(m), 0);

    pthread_t thread1, thread2;   
    pthread_create(&thread1, NULL, handleRecvMsg, NULL);
    pthread_create(&thread2, NULL, handleSendMsg, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    exit(0);
}