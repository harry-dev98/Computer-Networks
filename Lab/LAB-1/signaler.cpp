#include <bits/stdc++.h>
#include <signal.h> // kill
#include <fcntl.h> // O_*
#include <unistd.h> // read

using namespace std;

#define SIG "/tmp/signalTod"

int itr = 0;
int main(){
    int fd = open(SIG, O_RDONLY);
    int pid;
    read(fd, &pid, sizeof(int));
    int x;
    while(1){
        cout<<"Enter 1 to send interupt, else will exit\n";
        cin>>x;
        if(x!=1){
            return 0;
        }
        if(itr == 0){
            kill(pid, SIGUSR1);
        } else {
            kill(pid, SIGUSR2);
        }
        itr = 1 - itr;
    }
    return 0;
}