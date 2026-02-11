#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/un.h>
#include<string.h>

#define SOCKET_PATH "dns_socket"

// Database structure
struct DNS_Record {
    char domain[50];
    char ip[20];
};

// Hardcoded database for demonstration
struct DNS_Record db[] = {
    {"www.google.com", "142.250.183.68"},
    {"www.youtube.com", "172.217.166.46"},
    {"www.manipal.edu", "104.18.2.35"},
    {"localhost", "127.0.0.1"}
};
int db_size = 4;

int main()
{
    int sockfd, newsockfd, retval, i;
    socklen_t clilen;
    struct sockaddr_un serveraddr, clientaddr;
    char buffer[100];

    // Create Socket (AF_UNIX for Unix Domain)
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        printf("\nSocket Creation Error");
        return 0;
    }

    // Unlink the path if it exists to avoid "Address already in use"
    unlink(SOCKET_PATH);

    serveraddr.sun_family = AF_UNIX;
    strcpy(serveraddr.sun_path, SOCKET_PATH);

    retval = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if(retval == -1)
    {
        printf("\nBinding Error");
        close(sockfd);
        return 0;
    }

    retval = listen(sockfd, 5);
    if(retval == -1)
    {
        printf("\nListen Error");
        close(sockfd);
        return 0;
    }

    printf("DNS Server (UNIX Socket) Started...\n");

    while(1)
    {
        clilen = sizeof(clientaddr);
        newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &clilen);
        
        if(newsockfd == -1)
        {
            printf("\nAccept Error");
            continue;
        }

        // Receive Domain Name
        memset(buffer, '\0', sizeof(buffer));
        recv(newsockfd, buffer, sizeof(buffer), 0);
        printf("\nRequest Received for: %s", buffer);

        // Search in Database
        char response[100] = "Domain not found";
        for(i = 0; i < db_size; i++)
        {
            if(strcmp(buffer, db[i].domain) == 0)
            {
                strcpy(response, db[i].ip);
                break;
            }
        }

        // Send IP back
        send(newsockfd, response, sizeof(response), 0);
        close(newsockfd);
    }

    close(sockfd);
    unlink(SOCKET_PATH);
    return 0;
}