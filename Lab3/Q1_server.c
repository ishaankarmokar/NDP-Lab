#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>


int main(){
    int port_num;
    printf("Enter the port number: ");
    scanf("%d",&port_num);

    struct sockaddr_in client, server;

    int s=socket(AF_INET,SOCK_STREAM,0);

    if(s==-1){
        printf("\nSocket Creation failed\n");
        return 0;
    }

    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_family=AF_INET;
    server.sin_port=htons(port_num);

    printf("\nSocket created\n");

    int r=bind(s,(struct sockaddr*)&server,sizeof(server));

    if(r==-1){
        printf("\nBinding failed\n");
        close(s);
        return 0;
    }
    printf("\nBinding successful\n");

    int l=listen(s,1);
    if(l==-1){
        printf("Server listening issues\n");
        return 0;
    }
    printf("\nServer is listening\n");
    printf("Server is ready\n");
    socklen_t clen=sizeof(client);

    int acp=accept(s,(struct sockaddr*)&client,&clen);

    if(acp==-1){
        printf("\nFailed to connect to client\n");
        close(s);
        return 0;
    }
    printf("\nConnected to client\n");
    char buff[1024];
    int pid=fork();

    while(1){
        if(pid==0){
            //child process
            int recack=recv(acp,buff,sizeof(buff)-1,0);
            if(recack==-1){
                printf("Message receive failed\n");
                close(acp);
                close(s);
                return 0;
            }
            buff[recack]='\0';
            printf("\nPID: %d\nMessage Received : %s\n",getpid(),buff);

            if(strcmp(buff,"BYE")==0){
                printf("\nExiting communication\n");
                close(acp);
                close(s);
                return 0;
            }

        }else{
            //parent process
            char msg[1024];
            printf("Enter the message to send: ");
            fflush(stdout);
            fgets(msg, sizeof(msg), stdin);
            msg[strcspn(msg, "\n")] = 0;
            int sendack=send(acp,msg,strlen(msg)+1,0);
            if(sendack==-1){
                printf("\nMessage send failed\n");
                close(acp);
                close(s);
                return 0;
            }
            printf("\nPID: %d\nMessage Sent\n",getpid());
            if(strcmp(msg,"BYE")==0){
                printf("\nEnding commuication\n");
                close(acp);
                close(s);
                return 0;
            }
        }
    }
    close(acp);
    close(s);
    return 0;
}