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
int khem=0;
int fd[2];//File descriptor for creating a pipe
char arr1[80],arr2[80]; 
sem_t *psem; 
//This function continously reads fd[0] for any input data byte
//If available, prints

void *reader(void*)
{

 int     count = 0;
 sleep(1);
  //Delay in starting the reading from the pipe
    while(1){
        sem_wait(psem);
        char ch[80];
        int     result;
        read(fd[0], arr2, 80); 
        FILE *verdict_file_pointer;
        FILE *expected_output_file_pointer;
        char code_output_text_str[80];
        char expected_output_text_str[80];
        int stdin_save;
        char command[1024];
        printf("c++ -pthread %s -o out2", arr2);
        sprintf(command,"c++ -pthread -o out2 %s", arr2);
        int have_compilation_error = system(command);
        if(have_compilation_error>0){
            int verdict_fd = open("verdict1.txt",O_CREAT | O_RDWR);
            write(verdict_fd,"Compilation Error\n",18);
            fflush(stdout);
        }
        else{
            char code_output_text_str[80];
            char exected_output_text_str[80];
            int stdin_save;
            stdin_save = dup(STDIN_FILENO); 
            int input_text_fd  = open("in.txt",O_CREAT | O_RDWR);
            dup2(input_text_fd,STDIN_FILENO);
            close(input_text_fd);
            FILE* code_output_file_pointer = popen("./out2","r");
            int code_output_text_fd = fileno(code_output_file_pointer);
            dup2(code_output_text_fd,STDIN_FILENO);
            close(code_output_text_fd);
            expected_output_file_pointer = fopen("out.txt", "r"); 
            while (!feof(expected_output_file_pointer))  
            { 
                fscanf(expected_output_file_pointer,"%s",expected_output_text_str);
                fflush(stdin);
                scanf("%s",code_output_text_str); 
                fflush(stdin);
                if(strcmp(expected_output_text_str,code_output_text_str)){
                    verdict_file_pointer = fopen("verdict1.txt", "w"); 
                    fprintf(verdict_file_pointer,"Wrong Anwer\n");
                    printf("Wrong Anwer\n");
                    fflush(stdout);
                    return 0;
                }
            } 
            dup2(stdin_save, STDIN_FILENO); 
            close(stdin_save);
            verdict_file_pointer = fopen("verdict1.txt", "w"); 
            fprintf(verdict_file_pointer,"Wow...It's AC\n");
            printf("Wow...It's AC\n");
            fflush(stdout);
            exit(0);
        }
      sleep(1);
  }
}

//This function continously writes Alphabet into fd[1]

void *writer(void*)
{
    char * myfifo1 = "/tmp/myfifo1"; 
    mkfifo(myfifo1, 0666); 
    char arr1[80],arr2[80]; 
    system("gnome-terminal -- ./out");
    int fd1 = open(myfifo1, O_RDONLY); 
    while(1){
        read(fd1, arr1, sizeof(arr1));
        if(arr1[0]=='e' && arr1[1]=='x'){
            break;
        } 
        printf("User2: %s\n", arr1); 
        write(fd[1], arr1, 80); 
        close(fd[1]);
        sem_post(psem);
        sleep(1);
        wait(NULL);
    }
    unlink(myfifo1);
}


int main()
{
    sem_unlink("/sem");
    psem = sem_open("/sem", O_CREAT | O_RDWR, 0660, 1);
    if (pipe(fd) < 0) 
		exit(1); 
    pthread_t tid1,tid2;
    pthread_create(&tid1,NULL,writer,NULL);
    pthread_create(&tid2,NULL,reader,NULL);

    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    
}
