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

#define MAINCHANNEL "/tmp/mainfifo-1"
#define BUFF 10
#define MSG 10

struct msg {
    int type;
    char msg[MSG];
};

std::set<int> cData;
int msgqid1, msgqid2, pid;

void* handleSendMsg(void* arg){
    msg m;
    while(1){
        printf("Please enter message of less than 10 char\n");
        scanf("%s", m.msg);
        msgsnd(msgqid1, &m, sizeof(m), 0);
    }
    printf("Exits\n");
    pthread_exit(0);
}

void* handleRecvMsg(void* arg){
    msg m;
    while(1){
        msgrcv(msgqid1, &m, sizeof(m), pid, MSG_EXCEPT);
        std::cout<<"msg recieved from: "<<m.type<<"\n";
        std::cout<<"msg :"<<m.msg<<"\n";
    }
}

int main(){
    system("touch ./server-client1.txt");
    system("touch ./server-client2.txt");
    key_t key1 = ftok("./server-client1.txt", 1);
    key_t key2 = ftok("./server-client2.txt", 1);
    msgqid1 = msgget(key1, IPC_CREAT|0666);
    msgqid2 = msgget(key2, IPC_CREAT|0666);

    // sending my pid
    pid = getpid();
    msg m;
    m.type=pid;
    msgsnd(msgqid2, &m, sizeof(m), 0);
    pthread_t thread1, thread2;   
    pthread_create(&thread1, NULL, handleRecvMsg, NULL);
    pthread_create(&thread2, NULL, handleSendMsg, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    exit(0);
}