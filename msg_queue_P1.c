s
#include<stdio.h> 
#include<sys/ipc.h> 
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>
#include<sys/msg.h> 
#include<sys/stat.h>
#include<string.h>
struct msg_buffer 
{ 
    long msg_type; 
    char msg_text[100]; 
}message; 
int main()
{
	char *myfifo1="/tmp/myfifo1";
	mkfifo(myfifo1,0666);
	int pid2,pid3;
	int fd1=open(myfifo1,O_RDONLY);
	read(fd1,&pid2,1024);
	printf("%d\n",pid2);
	char *myfifo2="/tmp/myfifo2";
	mkfifo(myfifo2,0666);
	int fd2=open(myfifo2,O_RDONLY);
	read(fd2,&pid3,1024);
	printf("%d\n",pid3);
	key_t key=ftok("msg_queue",100);
	int msgid=msgget(key,0666 | IPC_CREAT);
	int n;
	printf("Enter the total no. of messages you want to send from P1 : ");
	scanf("%d",&n);
	while(n--)
	{
		char s[100];
		printf("To which process you wanna send the message : ");
		scanf("%s",s);
		if(s[1]=='2')
		{
			printf("Enter data for sending to P2 : ");
			char buf[100];
			scanf("%s",buf);
			strcpy(message.msg_text,buf);
			message.msg_type=pid2;
			msgsnd(msgid,&message,sizeof(message),0);
			printf("Data sent by P1 to P2 is %s\n",message.msg_text);
		}
		else
		{
			printf("Enter data for sending to P3 : ");
			char buf[100];
			scanf("%s",buf);
			strcpy(message.msg_text,buf);
			message.msg_type=pid3;
			msgsnd(msgid,&message,sizeof(message),0);
			printf("Data sent by P1 to P3 is %s\n",message.msg_text);
		}
	}
	strcpy(message.msg_text,"end");
	message.msg_type=pid2;
	msgsnd(msgid,&message,sizeof(message),0);
	strcpy(message.msg_text,"end");
	message.msg_type=pid3;
	msgsnd(msgid,&message,sizeof(message),0);
}
