#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define MAXSIZE 100

int main()
{
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;
    char buffer[MAXSIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        printf("\nSocket Creation Error");
        return 0;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = htons(INADDR_ANY);

    retval = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if(retval == -1)
    {
        printf("Binding error");
        close(sockfd);
        return 0;
    }

    retval = listen(sockfd, 5);
    if(retval == -1)
    {
        close(sockfd);
        return 0;
    }

    printf("Parity Check Server Ready...\n");

    while(1)
    {
        actuallen = sizeof(clientaddr);
        newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);

        if(newsockfd == -1)
        {
            close(sockfd);
            continue;
        }

        memset(buffer, '\0', sizeof(buffer));
        recv(newsockfd, buffer, sizeof(buffer), 0);
        
        printf("\nReceived Data Frame: %s", buffer);

        // Count number of 1s in the frame
        int ones = 0, i;
        int len = strlen(buffer);
        for(i = 0; i < len; i++)
        {
            if(buffer[i] == '1')
                ones++;
        }

        // Even Parity Check: Total 1s must be Even
        if(ones % 2 == 0)
        {
            printf("\nStatus: Data Accepted (No Error)\n");
            send(newsockfd, "Data Accepted", 14, 0);
        }
        else
        {
            printf("\nStatus: Data Corrupted (Parity Mismatch)\n");
            send(newsockfd, "Data Corrupted", 15, 0);
        }

        close(newsockfd);
    }
    close(sockfd);
}