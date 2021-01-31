#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>

#define MAINCHANNEL "/tmp/mainfifo-1"
#define BUFF 10

int NClients = 0;
std::map<pthread_t, int> cData;

bool compareFiles(FILE *fp1, FILE *fp2){ 
    // fetching character of two file 
    // in two variable ch1 and ch2 
    char ch1 = getc(fp1); 
    char ch2 = getc(fp2); 
    // error keeps track of number of errors 
    int error = 0; 
    // iterate loop till end of file 
    while (ch1 != EOF && ch2 != EOF){ 
        printf("%c",ch1); 
        // if fetched data is not equal then 
        // error is incremented 
        if (ch1 != ch2){ 
            error++; 
            printf("error");
        }
        // fetching character until end of file 
        ch1 = getc(fp1); 
        ch2 = getc(fp2); 
    }
    return !error;
}

void* handleClient(void *args){
    std::string writeDir = "/tmp/w";
    char* buff = (char *)args;
    char uname[4];
    uname[0] = *buff++;
    uname[1] = *buff++;
    uname[2] = *buff++;
    uname[3] = 0;
    char command[1024];
    sprintf(command, "c++ %s -o %s", buff, uname);
    printf("command: %s\n", command);
    char* wDir = strcat((char *)writeDir.c_str(), uname);
    int f1 = mkfifo(wDir, 0666);
    perror("thread: mkfifo wDir");
    int sendFD = open(wDir, O_WRONLY);
    perror("thread: sendFD");
    
    int exe = system(command);
    if(exe > 0){
        write(sendFD, "Error", 5);
        close(sendFD);
        pthread_exit(0);
    }
    int testcase_fd  = open("testcase.txt",O_CREAT | O_RDWR);
    dup2(testcase_fd,STDIN_FILENO);
    close(testcase_fd);
    char exef[10];
    sprintf(exef, "./%s", uname);
    FILE* output_fptr = popen(exef,"r");
    FILE* solution_fptr = fopen("solution.txt", "r");
    if(compareFiles(output_fptr, solution_fptr)){
        write(sendFD, "--AC-", 5);
    } else {
        write(sendFD, "--WA-", 5);
    }
    close(sendFD);
    pthread_exit(0);
}

int main(){
    mkfifo(MAINCHANNEL, 0666);
    struct pollfd mainfd[1];
    mainfd[0].fd = open(MAINCHANNEL, O_RDONLY);
    mainfd[0].events = POLLIN;
    do {
        int ret = poll(mainfd, 1, -1);
        if (ret == -1) {
            perror ("poll");
            return 1;
        }
        if (mainfd[0].revents & POLLIN){
            char buff[20];
            pthread_t thread;
            int r = read(mainfd[0].fd, buff, 20);
            printf("recv new msg: \n%s\n", buff);
            pthread_create(&thread, NULL, handleClient, (void *)buff);
        } else if(mainfd[0].revents == POLLHUP){
            printf("All fd connected has shutdown\n");
            close(mainfd[0].fd);
            mainfd[0].fd = open(MAINCHANNEL, O_RDONLY);
        }
        else if(mainfd[0].revents & (POLLERR | POLLNVAL)) {
            printf("socket error\n");
            break;
        }
    } while(1);
    close(mainfd[0].fd);
    return 0;
}