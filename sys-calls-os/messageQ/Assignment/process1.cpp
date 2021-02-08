#include<bits/stdc++.h>
#include<sys/types.h> // hass diff types
#include<sys/ipc.h> 
#include<sys/msg.h> // has a msgget, msgcntl, msgsnd, msgrcv
using namespace std;

#define FTOK "/tmp"
#define MSG 1
struct msg {
    int type;
    char message[MSG];
};

void printStateOfQ(int msgqid){
    msqid_ds info;
    msgctl(msgqid, IPC_STAT, &info);
    cout<<"Last write time: "<<info.msg_stime<<" by process: "<<info.msg_lspid<<"\n";
    cout<<"Last read time: "<<info.msg_stime<<" by process: "<<info.msg_lrpid<<"\n";
    cout<<"Last update time: "<<info.msg_ctime<<"\n";
    cout<<"Total messages in queue are: "<<info.msg_qnum<<"\n";
}

void printAllQ(int msgqid){
    msqid_ds info;
    msgctl(msgqid, IPC_STAT, &info);
    msg* ptr = (msg*) info.__glibc_reserved5;
    cout<<ptr->message[0]<<"\n";
}

int main(){
    key_t key = ftok(FTOK, 187218);
    if(key == -1){
        cout<<"some error to get key.\n";
    }
    int msgqid = msgget(key, IPC_CREAT|IPC_EXCL); // if already created error
    msg m;
    m.type = 1;
    m.message[0] = '1';
    msgsnd(msgqid, &m, sizeof(m), 0);
    printAllQ(msgqid);
    printStateOfQ(msgqid);
    msgctl(msgqid, IPC_RMID, NULL); //deletes the msgq
}