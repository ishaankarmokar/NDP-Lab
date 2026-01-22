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
    pid_t pid;

    // 1. Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    printf("[Client] Socket created.\n");

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 2. Connect
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed");
        exit(1);
    }
    printf("[Client] Connected to server.\n");

    // 3. Fork to handle full duplex
    pid = fork();

    if (pid == 0) {
        // --- CHILD PROCESS: RECEIVING ---
        printf("[Client Child] PID: %d, PPID: %d - Handling Receiving\n", getpid(), getppid());
        while (1) {
            memset(buffer, 0, MAXSIZE);
            int recvStatus = recv(sockfd, buffer, sizeof(buffer), 0);
            if (recvStatus <= 0) {
                printf("\n[Client] Server disconnected.\n");
                break;
            }
            if (strcmp(buffer, "bye") == 0) {
                printf("\n[Client] Server said bye. Exiting.\n");
                break;
            }
            printf("\nServer: %sClient: ", buffer);
            fflush(stdout); // Force print
        }
        close(sockfd);
        exit(0);
    } 
    else {
        // --- PARENT PROCESS: SENDING ---
        printf("[Client Parent] PID: %d, PPID: %d - Handling Sending\n", getpid(), getppid());
        while (1) {
            printf("Client: ");
            memset(buffer, 0, MAXSIZE);
            fgets(buffer, MAXSIZE, stdin);
            buffer[strcspn(buffer, "\n")] = 0; // Remove newline

            send(sockfd, buffer, strlen(buffer), 0);

            if (strcmp(buffer, "bye") == 0) {
                printf("[Client] Exiting chat.\n");
                break;
            }
        }
        close(sockfd);
        exit(0);
    }
    return 0;
}