#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/un.h>
#include<string.h>

#define SOCKET_PATH "dns_socket"

int main()
{
    int sockfd, retval;
    struct sockaddr_un serveraddr;
    char buffer[100];

    // Create Socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        printf("\nSocket Creation Error");
        return 0;
    }

    serveraddr.sun_family = AF_UNIX;
    strcpy(serveraddr.sun_path, SOCKET_PATH);

    // Connect to Server
    retval = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if(retval == -1)
    {
        printf("\nConnection Error");
        close(sockfd);
        return 0;
    }

    printf("Enter Domain Name: ");
    scanf("%s", buffer);

    // Send Domain Name
    send(sockfd, buffer, strlen(buffer) + 1, 0);

    // Receive IP Address
    memset(buffer, '\0', sizeof(buffer));
    recv(sockfd, buffer, sizeof(buffer), 0);

    printf("Resolved IP: %s\n", buffer);

    close(sockfd);
    return 0;
}