#include <bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/poll.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

using namespace std;
#define SCH 1
#define FROMS "/tmp/froms"
#define FROMC "/tmp/fromc"
#define SIG "/tmp/signalTod"

pthread_t th;
int sigCount = 0;

char buffer[1024];
int idx = 0;

int main(){
    mkfifo(FROMS, 0777);
    char s = 'a';
    FILE* w = popen(FROMS, "w");
    fputc(s, w);
    pclose(w);
    FILE* r = popen(FROMS, "r");
    int buff = fgetc(r);
    cout<<(char)buff<<"\n";
    pclose(r);
    return 0;
}