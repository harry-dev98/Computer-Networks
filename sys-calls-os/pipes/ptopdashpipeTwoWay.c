#include <unistd.h> // pipe, fork
#include <stdlib.h> // exit
#include <stdio.h> // perror
#include <sys/types.h> //pid_t
#include <semaphore.h> // sem_open etc
#include <fcntl.h> // O_*
#include <sys/wait.h> // wait


int main(){
    int pipeFD[2];
    if(pipe(pipeFD) == -1){
        perror("pipe error");
        exit(1);
    }
    sem_t *sem1, *sem2;
    sem1 = sem_open("SEM--1", O_CREAT|O_EXCL, 0644, 0);
    sem2 = sem_open("SEM--2", O_CREAT|O_EXCL, 0644, 0);
    sem_unlink("SEM--1");
    sem_unlink("SEM--2");
    pid_t pid = fork();
    if(pid < 0){
        perror("no forking");
        exit(1);
    } else if(pid == 0){
        char buffer[10];
        sem_wait(sem1);
        read(pipeFD[0], buffer, 10);
        printf("read code form parent: %s\n", buffer);
        printf("processed the code, now returning response\n");
        buffer[0] = '9';
        write(pipeFD[1], buffer, 10);
        sem_post(sem2);
        exit(0);
    } else {
        char buffer[10];
        char *secret = "123456789\0";
        write(pipeFD[1], secret, 10);
        printf("sent secret to child process to read\n");
        sem_post(sem1);
        printf("child process signaled to read\n");
        sem_wait(sem2);
        read(pipeFD[0], buffer, 10);
        printf("read response from child: %s\n", buffer);
        wait(NULL);
    }
    sem_destroy(sem1);
    sem_destroy(sem2);
    exit(0);
}