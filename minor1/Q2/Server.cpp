#include<bits/stdc++.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
using namespace std;

#define Client "/tmp/ESS"

int main(){
    system("touch /tmp/ESS");
    if(!mkfifo(Client, 0666)){
        int fd = open(Client, O_RDONLY);
        char buff;
        while(read(fd, &buff, sizeof(char))){
            cout<<buff<<"\n";
        }
    } else {
        cout<<"mkfifo error\n";
    }
}