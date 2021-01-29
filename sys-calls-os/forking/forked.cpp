#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

int main(){
    cout<<"This is main process running\n";
    pid_t pid = fork();
    if(pid == 0){
        for(int i=0; i<1000; i++);
        cout<<"this is a child process\n";
    } else if(pid > 0){
        for(int i=0; i<1000; i++);
        cout<<"parent process done, now waits for child to exit\n";
        wait(NULL);
    } else {
        perror("fork not success");
    }
    cout<<pid<<" process is exiting\n";
    return 0;
}