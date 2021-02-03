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
#define FROMC "/tmp/from-c"

int main(){
    int fd = open(FROMC, O_WRONLY);
    perror("fd");
    int t = 20;
    while(t--){
        sleep(1);
        char s = '1';
        // cin>>s;
        write(fd, &s, sizeof(char));
        cout<<"sent "<<s<<"\n";
    }
    return 0;
}