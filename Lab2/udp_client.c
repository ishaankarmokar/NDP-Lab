#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main(){
    int port_num;
    printf("Enter port number: ");
    scanf("%d",&port_num);

    struct sockaddr_in server;
    socklen_t slen = sizeof(server);

    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if(s == -1){
        printf("Socket error\n");
        return 0;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port_num);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    char fname[50];
    printf("Enter file name: ");
    scanf("%s",fname);

    sendto(s, fname, sizeof(fname), 0, (struct sockaddr*)&server, slen);

    char reply[50];
    recvfrom(s, reply, sizeof(reply), 0, NULL, NULL);
    printf("%s\n", reply);

    if(strcmp(reply,"File not present")==0){
        close(s);
        return 0;
    }

    while(1){
        int ch;
        printf("\n1.Search 2.Replace 3.Reorder 4.Exit\n");
        scanf("%d",&ch);

        char opt[10];
        sprintf(opt,"%d",ch);
        sendto(s, opt, sizeof(opt), 0, (struct sockaddr*)&server, slen);

        if(ch==1){
            char word[50];
            printf("Enter string: ");
            scanf("%s",word);
            sendto(s, word, sizeof(word), 0, (struct sockaddr*)&server, slen);
            recvfrom(s, reply, sizeof(reply), 0, NULL, NULL);
            printf("%s\n",reply);
        }

        else if(ch==2){
            char s1[50], s2[50];
            printf("Old string: ");
            scanf("%s",s1);
            printf("New string: ");
            scanf("%s",s2);
            sendto(s, s1, sizeof(s1), 0, (struct sockaddr*)&server, slen);
            sendto(s, s2, sizeof(s2), 0, (struct sockaddr*)&server, slen);
            recvfrom(s, reply, sizeof(reply), 0, NULL, NULL);
            printf("%s\n",reply);
        }

        else if(ch==3){
            recvfrom(s, reply, sizeof(reply), 0, NULL, NULL);
            printf("%s\n",reply);
        }

        else if(ch==4){
            break;
        }
    }

    close(s);
}