#include<bits/stdc++.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
using namespace std;

#define Server "/tmp/ESS"
#define ClientConn "/tmp/Client1"
#define ClientConn1 "/tmp/Client2"

vector<pid_t> Clients;

void pingESS(){
    int fd = open(Server, O_WRONLY);
    char buff = 'a';
    write(fd, &buff, sizeof(char));
    close(fd);
}

int main(){
    mkfifo(ClientConn, 0666);
    mkfifo(ClientConn1, 0666);
    

}