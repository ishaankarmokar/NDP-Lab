#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>

#define MAXSIZE 100

int main()
{
    int sockfd, retval;
    struct sockaddr_in serveraddr;
    char buff[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        printf("\nSocket Creation Error");
        return 0;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retval = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if(retval == -1)
    {
        printf("Connection error");
        return 0;
    }

    // Check if server sent a terminate command immediately
    // Use MSG_DONTWAIT or a small check, but for lab simplicity
    // we assume we send first unless the server is full.
    
    printf("Enter the text: ");
    // Using gets or scan with %[^\n] to allow spaces
    scanf(" %[^\n]", buff); 

    send(sockfd, buff, sizeof(buff), 0);
    
    // Check for termination message
    memset(buff, '\0', sizeof(buff));
    recv(sockfd, buff, sizeof(buff), 0);
    
    if(strcmp(buff, "terminate session") == 0)
    {
        printf("\nServer sent: %s\nExiting...\n", buff);
    }
    else
    {
        printf("\nData sent successfully.\n");
    }

    close(sockfd);
    return 0;
}