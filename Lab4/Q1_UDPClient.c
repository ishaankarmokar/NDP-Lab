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
    
    server.sin_family = AF_INET;
    server.sin_port = htons(port_num);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    while(1){
        int ch;
        printf("\n1. Registration Number\n2. Name of Student\n3. Subject Code\n4. Exit\nEnter option: ");
        scanf("%d", &ch);

        if(ch == 4) break;

        char input[50];
        if(ch == 1) printf("Enter Registration Number: ");
        else if(ch == 2) printf("Enter Student Name: ");
        else if(ch == 3) printf("Enter Subject Code: ");
        
        scanf("%s", input);

        sendto(s, &ch, sizeof(ch), 0, (struct sockaddr*)&server, slen);
        sendto(s, input, sizeof(input), 0, (struct sockaddr*)&server, slen);

        char reply[200];
        recvfrom(s, reply, sizeof(reply), 0, NULL, NULL);
        printf("\nServer Response:\n%s\n", reply);
    }

    close(s);
    return 0;
}