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
    long mtype;
    char msg[MSG];
};

std::set<int> cData;
int msgqid1, msgqid2, msgqid3, pid;

void* handleSendMsg(void* arg){
    msg m;
    while(1){
        printf("Please enter message of less than 10 char\n");
        scanf("%s", m.msg);
        m.mtype = pid;
        msgsnd(msgqid1, &m, sizeof(m), 0);
        perror("msgsnd");
    }
    printf("Exits\n");
    pthread_exit(0);
}

void* handleRecvMsg(void* arg){
    msg m;
    while(1){
        std::cout<<"waiting for some msg\n";
        msgrcv(msgqid3, &m, sizeof(m), pid, 0);
        perror("msgrcv");
        std::cout<<"msg recieved from: "<<m.mtype<<"\n";
        std::cout<<"msg :"<<m.msg<<"\n";
    }
    printf("Exits\n");
    pthread_exit(0);
}

int main(){
    system("touch ./server-client1.txt");
    system("touch ./server-client2.txt");
    system("touch ./server-client3.txt");
    key_t key1 = ftok("./server-client1.txt", 1);
    key_t key2 = ftok("./server-client2.txt", 1);
    key_t key3 = ftok("./server-client3.txt", 1);
    msgqid1 = msgget(key1, IPC_CREAT|0666);
    msgqid2 = msgget(key2, IPC_CREAT|0666);
    msgqid3 = msgget(key3, IPC_CREAT|0666);

    // sending my pid
    pid = getpid();
    msg m;
    m.mtype=pid;
    msgsnd(msgqid2, &m, sizeof(m), 0);

    pthread_t thread1, thread2;   
    pthread_create(&thread1, NULL, handleRecvMsg, NULL);
    pthread_create(&thread2, NULL, handleSendMsg, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    exit(0);
}