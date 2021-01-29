#include <stdio.h> // std i/o, perror
#include <stdlib.h> //exit(), malloc(), free()
#include <unistd.h> // fork()
#include <semaphore.h> // sem_unlink(), sem_open(), sem_wait(), sem_post(), etc
#include <sys/shm.h> // shmget(), shmat(), etc
#include <sys/types.h> // key_t, sem_t, pid_t, etc
#include <fcntl.h> // O_CREAT, O_EXEC, etc
#include <sys/wait.h> // wait(), etc.


int main(){
    key_t shmkey1 = ftok("/dev/null", 5);
    key_t shmkey2 = ftok("/dev/null", 6);
    int shmid1 = shmget(shmkey1, sizeof(int), 0644|IPC_CREAT);
    int shmid2 = shmget(shmkey2, sizeof(int), 0644|IPC_CREAT);
    if(shmid1 < 0 || shmid2 < 0){
        perror("shmget error");
        exit(1);
    }
    int *shm1 = (int *) shmat(shmid1, NULL, 0);
    int *shm2 = (int *) shmat(shmid2, NULL, 0);
    *shm1 = 0;
    *shm2 = 0;

    sem_t *sem1 = sem_open("SEM1", O_CREAT|O_EXCL, 0644, 1);
    sem_t *sem2 = sem_open("SEM2", O_CREAT|O_EXCL, 0644, 0);
    sem_unlink("SEM1");
    sem_unlink("SEM2");
    int pid = fork();
    if(pid < 0){
        perror("forking");
    } else if(pid == 0){
        sem_wait(sem2);
        printf("child process reads from shared memory 1, val = %d\n", *shm1);
        printf("child process writes to shared memory 2\n");
        *shm2 = 5678;
        sem_post(sem1);
    } else {
        sem_wait(sem1);
        printf("parent process writes to shared memory 1\n");
        *shm1 = 1234;
        sem_post(sem2);
        sem_wait(sem1);
        printf("parent process reads from shared memory 2, val = %d\n", *shm2);
        sem_post(sem2);
        wait(NULL);
    }
    shmdt (shm1);
    shmdt (shm2);
    shmctl (shmid1, IPC_RMID, 0);
    shmctl (shmid2, IPC_RMID, 0);
    sem_destroy(sem1);
    sem_destroy(sem2);
    exit(0);
}