#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main(){
    int port;
    printf("Enter port number: ");
    scanf("%d",&port);

    int s = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(s, (struct sockaddr*)&server, sizeof(server)) == -1){
        printf("Connection failed\n");
        close(s);
        return 0;
    }

    printf("Connected to server\n");

    char buff[1024], recvbuf[1024];

    while(1){
        printf("\nEnter alphanumeric string (or exit): ");
        scanf("%s", buff);

        send(s, buff, strlen(buff), 0);

        if(strcmp(buff,"exit")==0)
            break;

        int n = recv(s, recvbuf, sizeof(recvbuf)-1, 0);
        recvbuf[n] = '\0';
        printf("%s\n", recvbuf);

        n = recv(s, recvbuf, sizeof(recvbuf)-1, 0);
        recvbuf[n] = '\0';
        printf("%s\n", recvbuf);
    }

    close(s);
    return 0;
}