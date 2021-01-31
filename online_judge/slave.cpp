#include <stdio.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include<bits/stdc++.h>
#include <fstream> 
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/ipc.h> 
using namespace std; 

int main(){
    // key_t key = ftok("shmfile",65); 
	// int shmid = shmget(key,1024,0666|IPC_CREAT); 
	int *tn ; //(int*) shmat(shmid,(void*)0,0); 
    // printf("Terminal no. %d",++(*tn));
    char myfifo[80];
    sprintf(myfifo,"/tmp/myfifo%d", 1);
    mkfifo(myfifo, 0666); 
    char arr2[80]; 
    char *arr1 = "ex";
    int fd = open(myfifo, O_WRONLY); 
    while(1){
    fgets(arr2, 80, stdin); 
    write(fd, arr2, strlen(arr2)+1);
    if(arr2[0]=='e' && arr2[1]=='x'){
        break;
    }
    sleep(1);
    }
    close(fd);
    return 0;
}