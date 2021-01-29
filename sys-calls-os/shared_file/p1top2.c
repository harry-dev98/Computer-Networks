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
    len=read(fd, buff, BUFFSIZE);
    if(len<=0){ 
        printf("%d ends\n", getpid());
        return 0;
    }
    buff[len] = '\0';
    printf("%d reads: %s\n", getpid(), buff);
    return 1;
}

int main(){
    fd = open("text.txt", O_RDONLY);
    printf("fd: %d\n", fd);
    if(fd < 0){
        perror("file cant be opened");
        exit(1);
    } 
    sem_unlink("SEM1");
    sem_unlink("SEM2");
    sem_t *sem_1 = sem_open("SEM1", O_CREAT|O_EXCL, 0644, 1);
    sem_t *sem_2 = sem_open("SEM2", O_CREAT|O_EXCL, 0644, 0);
    pid_t pid = fork();

    if(pid == 0){
        dup2(fd, 0);
        char* args[] = {NULL, };
        execvp("./otherprocess",args);
    } else if(pid < 0){
        perror("no forking");
    } else {
        while(1){
            sem_wait(sem_2);
            bool flag = !readFile();   
            sem_post(sem_1);
            if(flag)break;
        }
    }

    close(fd);
    printf("fd: %d", fd);
    exit(0);
}