#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <fcntl.h>

sem_t *s;

void task()
{
    printf("%d process to access critical section \n", getpid());
    // wait here for worker thread to finish
    sem_wait(s);

    printf("%d started its task, and locked this statement \n", getpid());
    sleep(2);
    printf("%d done its task, and unlocked this statement \n", getpid());

    // signaling worker's task (printing) is done
    sem_post(s);
}

int main()
{
    s = sem_open ("pSem", O_CREAT | O_EXCL, 0644, 1);
    sem_unlink ("pSem");   
    printf("main process started\n");

    pid_t pid;
    int n = 4;
    for(int i=0; i<n; i++){
        pid = fork();
        if(pid == 0)break;
    }
    if(pid == 0){
        task();
    } else {
        for(int i=0; i<n; i++) wait(NULL);
        sem_destroy(s);
    }
    return 0;
}