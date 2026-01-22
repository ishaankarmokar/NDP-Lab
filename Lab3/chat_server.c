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
    int sockfd, newsockfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;
    char buffer[MAXSIZE];
    pid_t pid;

    // 1. Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    printf("[Server] Socket created.\n");

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 2. Bind
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        exit(1);
    }
    printf("[Server] Bound to port %d.\n", PORT);

    // 3. Listen
    if (listen(sockfd, 5) == 0) {
        printf("[Server] Listening...\n");
    } else {
        perror("Listen failed");
        exit(1);
    }

    // 4. Accept
    addr_size = sizeof(clientAddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &addr_size);
    if (newsockfd < 0) {
        perror("Accept failed");
        exit(1);
    }
    printf("[Server] Client connected.\n");

    // 5. Fork to handle full duplex
    pid = fork();

    if (pid == 0) {
        // --- CHILD PROCESS: RECEIVING ---
        printf("[Server Child] PID: %d, PPID: %d - Handling Receiving\n", getpid(), getppid());
        while (1) {
            memset(buffer, 0, MAXSIZE);
            int recvStatus = recv(newsockfd, buffer, sizeof(buffer), 0);
            if (recvStatus <= 0) {
                printf("\n[Server] Client disconnected.\n");
                break;
            }
            if (strcmp(buffer, "bye") == 0) {
                printf("\n[Server] Client said bye. Exiting.\n");
                break;
            }
            printf("\nClient: %sServer: ", buffer);
            fflush(stdout); // Force print to appear
        }
        close(newsockfd);
        exit(0);
    } 
    else {
        // --- PARENT PROCESS: SENDING ---
        printf("[Server Parent] PID: %d, PPID: %d - Handling Sending\n", getpid(), getppid());
        while (1) {
            printf("Server: ");
            memset(buffer, 0, MAXSIZE);
            fgets(buffer, MAXSIZE, stdin);
            buffer[strcspn(buffer, "\n")] = 0; // Remove newline char

            send(newsockfd, buffer, strlen(buffer), 0);

            if (strcmp(buffer, "bye") == 0) {
                printf("[Server] Exiting chat.\n");
                break;
            }
        }
        close(newsockfd);
        close(sockfd);
        exit(0);
    }
    return 0;
}