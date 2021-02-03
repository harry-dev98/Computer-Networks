// code to test if threads both belong to 
// same process group, and what process id they have

#include <bits/stdc++.h>
#include <pthread.h> // pthread
#include <unistd.h> // getpid, getpgid
using namespace std;

void * thread_work(void* arg){
    cout<<"thread created, has pid: "<<getpid()<<", pgid: "<<getpgid(0)<<"\n";
    pthread_exit(0);
}

int main(){
    cout<<"process started, has pid: "<<getpid()<<", pgid: "<<getpgid(0)<<"\n";
    pthread_t th1, th2;
    pthread_create(&th1, NULL, thread_work, NULL);
    pthread_create(&th2, NULL, thread_work, NULL);
    void* out1,* out2;
    pthread_join(th1, &out1);
    pthread_join(th2, &out2);
    return 0;
}