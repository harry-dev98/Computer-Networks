// code to test if parent child both belong to same process group

#include <bits/stdc++.h>
#include <unistd.h> // fork
#include <sys/wait.h> //wait
using namespace std;

int main(){
    int n = 10, pid;
    cout<<"Main process: "<<getpid()<<", with process group: "<<getpgid(0)<<".\n";
    for(int i=0; i<n; i++){
        pid = fork();
        if(pid<0){
            cout<<"no fork\n";
        } else if(pid == 0){
            cout<<"child "<<i<<" created with process group: "<<getpgid(0)<<".\n";
            break; 
        } else {
            wait(NULL);
        }
    }
    return 0;
}