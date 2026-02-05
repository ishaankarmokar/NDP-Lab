#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main(){
    int port_num;
    printf("Enter the port number: ");
    scanf("%d",&port_num);

    struct sockaddr_in server;
    int s=socket(AF_INET,SOCK_STREAM,0);

    if(s==-1){
        printf("\nSocket creation failed\n");
        return 0;
    }
    server.sin_addr.s_addr=inet_addr("127.0.0.1");
    server.sin_family=AF_INET;
    server.sin_port=htons(port_num);

    printf("\nSocket created successfully\n");

    int c=connect(s,(struct sockaddr*)&server,sizeof(server));

    if(c==-1){
        printf("\nConnection failed\n");
        close(s);
        return 0;
    }
    char buff[1024];
    int pid=fork();

    while(1){
        if(pid==0){
            //child process
            char msg[1024];
            printf("Enter the message to send: ");
            fflush(stdout);
            fgets(msg, sizeof(msg), stdin);
            msg[strcspn(msg, "\n")] = 0;
            int sendack=send(s,msg,strlen(msg)+1,0);
            if(sendack==-1){
                printf("\nMessage send failed\n");
                close(s);
                return 0;
            }
            printf("\nPID: %d\nMessage Sent\n",getpid());
            if(strcmp(msg,"BYE")==0){
                printf("\nEnding commuication\n");
                close(s);
                return 0;
            }
        }else{
            //parent process
            int recack=recv(s,buff,sizeof(buff)-1,0);
            if(recack==-1){
                printf("Message receive failed\n");
                close(s);
                return 0;
            }
            buff[recack]='\0';
            printf("\nPID: %d\nMessage Received : %s\n",getpid(),buff);

            if(strcmp(buff,"BYE")==0){
                printf("\nExiting communication\n");
                close(s);
                return 0;
            }
        }
    }
    close(s);
    return 0;
}