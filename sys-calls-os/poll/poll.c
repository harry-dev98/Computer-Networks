#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/poll.h>

#define FIFOFILE "/tmp/FIFOFILE"
#define TIMEOUT 100
#define BUFF 10

int main(){
    mkfifo(FIFOFILE, 0666);
    struct pollfd fds[1];
    int ret;

    /* watch stdin for input */
    fds[0].fd = open(FIFOFILE, O_RDONLY);
    fds[0].events = POLLIN;
    do {
        ret = poll(fds, 1, 100);
        printf("checking if poll, %d, %d\n", ret, fds[0].revents);
        if (ret == -1) {
            perror ("poll");
            return 1;
        }

        if (!ret) {
            printf ("%d seconds elapsed.\n", TIMEOUT);
            return 0;
        }
        if (fds[0].revents & POLLIN){
            printf ("stdin is readable: %d\n", ret);
            char buff[BUFF];
            read(fds[0].fd, buff, BUFF);
            printf("%s\n", buff);
        } 
        else if(fds[0].revents & POLLHUP){
            printf("All fd connected has shutdown\n");
            close(fds[0].fd);
            fds[0].fd = open(FIFOFILE, O_RDONLY);
        }
        else if(fds[0].revents & (POLLERR | POLLNVAL)) {
            printf("socket error\n");
            break;
        }
    } while(1);
    return 0;
}
