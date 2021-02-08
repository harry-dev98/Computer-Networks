#include<bits/stdc++.h>
#include<sys/types.h> // hass diff types
#include<sys/ipc.h> 
#include<sys/msg.h> // has a msgget, msgcntl, msgsnd, msgrcv
using namespace std;

#define FTOK "./tmpfile1.txt"
#define MSG 1
struct msg {
    int type;
    char message;
};

void printStateOfQ(int msgqid){
    msqid_ds info;
    msgctl(msgqid, IPC_STAT, &info);
    cout<<"Last write time: "<<info.msg_stime<<" by process: "<<info.msg_lspid<<"\n";
    cout<<"Last read time: "<<info.msg_stime<<" by process: "<<info.msg_lrpid<<"\n";
    cout<<"Last update time: "<<info.msg_ctime<<"\n";
    cout<<"Total messages in queue are: "<<info.msg_qnum<<"\n";
}

// void printAllQ(int msgqid){
//     msqid_ds info;
//     msgctl(msgqid, IPC_STAT, &info);
//     msg* ptr = (msg*) info.__glibc_reserved5;
//     cout<<ptr->message[0]<<"\n";
// }

int main(){
    system("touch ./tmpfile1.txt");
    key_t key = ftok(FTOK, 1);
    perror("ftok");
    if(key == -1){
        cout<<"some error to get key.\n";
    }
    int msgqid = msgget(key, IPC_CREAT|0666); // if already created error
    perror("msget");
    msg m, m2;
    m.type = 1;
    m.message = '1';
    msgsnd(msgqid, &m, sizeof(m), 0);
    perror("snd");
    printStateOfQ(msgqid);
    msgrcv(msgqid, &m2, sizeof(m), 0, 0);
    perror("recv");
    cout<<"recv: "<<m2.message<<"\n";
    msgctl(msgqid, IPC_RMID, NULL); //deletes the msgq
    exit(0);
}