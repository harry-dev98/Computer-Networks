#include<bits/stdc++.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<unistd.h>
#include<netinet/in.h> // sockaddr_in
#include<arpa/inet.h> // inet_pton

using namespace std;

void _error(string s){
    perror(s.c_str());
    exit(0);
}

int main(){
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd < 0){
        _error("socket");
    }
    sockaddr_in addr;
    addr.sin_port = htons(6910);
    addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr)<=0){
        _error("can't convert");
    }
    if(connect(sfd, (sockaddr *)&addr, sizeof(addr))){
        _error("can't connect");
    }
    char buff[1024];
    recv(sfd, &buff, 1024, 0);
    cout<<buff<<"\n";
    close(sfd);
}