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
    sem_t *sem;
    sem = sem_open("SEM", O_CREAT|O_EXCL, 0644, 0);
    sem_unlink("SEM");
    char buffer[10];
    pid_t pid = fork();
    if(pid < 0){
        perror("no forking");
        exit(1);
    } else if(pid == 0){
        sem_wait(sem);
        read(pipeFD[0], buffer, 10);
        printf("read: %s\n", buffer);
        exit(0);
    } else {
        char *secret = "123456789\0";
        printf("send secret to child process to read\n");
        write(pipeFD[1], secret, 10);
        sem_post(sem);
        wait(NULL);
    }
    exit(0);
}