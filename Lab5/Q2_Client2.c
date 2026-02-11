#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define MAXSIZE 100

int main()
{
    int sockfd, retval;
    struct sockaddr_in serveraddr;
    char str1[MAXSIZE], str2[MAXSIZE];
    char reply[MAXSIZE];

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
        close(sockfd);
        return 0;
    }

    printf("Connected to Server.\n");

    // Input strings
    printf("Enter first string: ");
    scanf("%s", str1);
    printf("Enter second string: ");
    scanf("%s", str2);

    // Send strings
    send(sockfd, str1, sizeof(str1), 0);
    send(sockfd, str2, sizeof(str2), 0);

    // Receive result
    memset(reply, 0, sizeof(reply));
    recv(sockfd, reply, sizeof(reply), 0);

    printf("\nServer Reply: %s\n", reply);

    close(sockfd);
    return 0;
}