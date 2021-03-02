#include<bits/stdc++.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<unistd.h>
#include<netinet/in.h> // sockaddr_in

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
    int optval_get;
    int optval_sz = sizeof(int);
    int optval = 1;
    int sopt;
    sopt = getsockopt(sfd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, (void *)&optval, (socklen_t *)&optval_sz);
    if(sopt == -1){
        perror("get socket opt");
    } else {
        cout<<"socket val:" <<optval_get;
    }
    sopt = setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, (void *)&optval, sizeof(optval));
    if(sopt == -1){
        perror("set socket opt to 1");
    }

    sockaddr_in addr;
    addr.sin_port = htons(6910);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sfd, (sockaddr *)&addr, sizeof(addr))){
        _error("can't bind");
    }

    if(listen(sfd, 5)){
        _error("can't listen");
    }

    while(true){
        sockaddr_in peerAddr;
        int peerAddrSz = sizeof(peerAddr);
        int newSock = accept(sfd, (sockaddr*)&peerAddr, (socklen_t *)&peerAddrSz);
        if(newSock == -1){
            _error("can't accept");
        }
        string welcome = "Hello! "+ to_string(peerAddr.sin_addr.s_addr) +" Welcome to my server.";
        cout<<"sending message\n";
        send(newSock, (void *)&welcome, sizeof(welcome), 0);
        close(newSock);
    }

}