#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 3388
#define MAXSIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[MAXSIZE];

    // 1. Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 2. Connect
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed");
        exit(1);
    }
    printf("Connected to server.\n");

    // 3. Input String
    printf("Input string: ");
    scanf("%s", buffer);

    // 4. Send to Server
    send(sockfd, buffer, strlen(buffer), 0);

    // 5. Receive Results
    // We expect two responses (one from child, one from parent).
    // Loop twice to get both.
    printf("At the server side:\n");
    
    for(int i=0; i<2; i++) {
        memset(buffer, 0, MAXSIZE);
        int bytesReceived = recv(sockfd, buffer, MAXSIZE, 0);
        
        if (bytesReceived > 0) {
            printf("%s\n", buffer);
        } else {
            break;
        }
    }

    close(sockfd);
    return 0;
}