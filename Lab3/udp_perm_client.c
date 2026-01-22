#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4444
#define MAXSIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[MAXSIZE];
    socklen_t addr_size;

    // 1. Create UDP Socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (1) {
        printf("\nEnter string (or 'exit'): ");
        scanf("%s", buffer);

        // 2. Send to Server
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // 3. Wait for Server Response (Half Duplex enforcement)
        printf("[Client] Waiting for server processing...\n");
        addr_size = sizeof(serverAddr);
        memset(buffer, 0, MAXSIZE);
        recvfrom(sockfd, buffer, MAXSIZE, 0, (struct sockaddr*)&serverAddr, &addr_size);

        printf("[Server Reply]: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}