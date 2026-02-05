#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>

int main(){
    int port_num;
    printf("Enter port number: ");
    scanf("%d",&port_num);

    struct sockaddr_in server;

    int s=socket(AF_INET,SOCK_STREAM,0);
    if(s==-1){
        printf("\nSocket error\n");
        return 0;
    }
    printf("\nSocket Created Successfully\n");

    server.sin_family=AF_INET;
    server.sin_port=htons(port_num);
    server.sin_addr.s_addr=inet_addr("127.0.0.1");

    if(connect(s,(struct sockaddr*)&server,sizeof(server))==-1){
        close(s);
        return 0;
    }
    printf("\nConnected to Server\n");

    char fname[50];
    printf("Enter file name: ");
    scanf("%s",fname);
    send(s,fname,sizeof(fname),0);

    char reply[50];
    recv(s,reply,sizeof(reply),0);

    printf("%s",reply);

    while(1){
        int ch;
        printf("\n1.Search 2.Replace 3.Reorder 4.Exit\n");
        scanf("%d",&ch);

        char opt[10];
        sprintf(opt,"%d",ch);
        send(s,opt,sizeof(opt),0);

        if(ch==1){
            char word[50];
            printf("Enter string: ");
            scanf("%s",word);
            send(s,word,sizeof(word),0);
            recv(s,reply,sizeof(reply),0);
            printf("%s\n",reply);
        }

        else if(ch==2){
            char s1[50], s2[50];
            printf("Old string: ");
            scanf("%s",s1);
            printf("New string: ");
            scanf("%s",s2);
            send(s,s1,sizeof(s1),0);
            send(s,s2,sizeof(s2),0);
            recv(s,reply,sizeof(reply),0);
            printf("%s\n",reply);
        }

        else if(ch==3){
            recv(s,reply,sizeof(reply),0);
            printf("%s\n",reply);
        }

        else if(ch==4){
            break;
        }
    }

    close(s);
}