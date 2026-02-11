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
    server.sin_family = AF_INET;
    server.sin_port = htons(port_num);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    while(1){
        int ch;
        printf("\n1. Registration Number\n2. Name of Student\n3. Subject Code\n4. Exit\nEnter option: ");
        scanf("%d", &ch);

        if(ch == 4) break;

        // Create socket for this request
        int s = socket(AF_INET, SOCK_STREAM, 0);
        if(connect(s, (struct sockaddr*)&server, sizeof(server)) == -1){
            printf("Connection failed\n");
            close(s);
            break;
        }

        char input[50];
        if(ch == 1) printf("Enter Registration Number: ");
        else if(ch == 2) printf("Enter Student Name: ");
        else if(ch == 3) printf("Enter Subject Code: ");
        
        scanf("%s", input);

        // Send Option and Details
        send(s, &ch, sizeof(ch), 0);
        send(s, input, sizeof(input), 0);

        char reply[200];
        recv(s, reply, sizeof(reply), 0);
        printf("\nServer Response:\n%s\n", reply);

        close(s);
    }
    return 0;
}