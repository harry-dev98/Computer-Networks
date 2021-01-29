#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>
#include <wait.h>
#include <stdbool.h>

#define BUFFSIZE 16
char buff[BUFFSIZE+1];
int len, fd;

int readFile(){
    len=read(0, buff, BUFFSIZE);
    if(len<=0){ 
        printf("%d ends\n", getpid());
        return 0;
    }
    buff[len] = '\0';
    printf("%d reads: %s\n", getpid(), buff);
    return 1;
}

int main(){
    sem_t *sem_1 = sem_open("SEM1", O_RDWR);
    sem_t *sem_2 = sem_open("SEM2", O_RDWR);
    while(1){
        sem_wait(sem_1);
        bool flag = !readFile();  
        sem_post(sem_2);
        if(flag)break;
    }
    exit(0);
}