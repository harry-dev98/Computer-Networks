#include <unistd.h> // pipe
#include <fcntl.h> // O_* flags
#include <stdio.h> // perror
#include <stdlib.h> // exit

int main(){
    int pipeFD[2];
    char buffer[10];
    if(pipe(pipeFD) == -1){
        perror("no pipe");
        exit(1);
    }
    char *pin = "123456789\0";
    printf("now writing to pipe\n");
    write(pipeFD[1], pin, 10);
    printf("now reading from pipe\n");
    read(pipeFD[0], buffer, 10);
    printf("read: %s\n", buffer);
    exit(0);
}