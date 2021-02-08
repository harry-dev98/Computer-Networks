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
int msgqid1, msgqid2;

void* handleClient(void *args){
    msg* buff = (msg *)args;
    printf("thread: pid recv: %d\n", buff->type);
    cData.insert(buff->type);
    do {
        msg m;
        msgrcv(msgqid1, &m, sizeof(m), buff->type, 0);
        for(auto cD: cData){
            m.type = cD;
            msgsnd(msgqid1, &m, sizeof(m), 0);
        }
    } while(1);
    printf("Exits\n");
    pthread_exit(0);

}

int main(){
    system("touch ./server-client1.txt");
    system("touch ./server-client2.txt");
    key_t key1 = ftok("./server-client1.txt", 1);
    key_t key2 = ftok("./server-client2.txt", 1);
    msgqid1 = msgget(key1, IPC_CREAT|0666);
    msgqid2 = msgget(key2, IPC_CREAT|0666);

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