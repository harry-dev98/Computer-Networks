#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;

int main(){
    cout<<"This is process 1 running\n";
    pid_t pid = fork();
    if(pid == 0){
        char* const args[]= {NULL, }; 
        execvp("./otherProcess", args); 

    } else if(pid > 0){
        wait(NULL);
    } else {
        perror("fork not success");
    }
    cout<<"This is process 1 exiting\n";
    return 0;
}