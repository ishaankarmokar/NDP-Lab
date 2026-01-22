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

// Helper function to swap characters
void swap(char *x, char *y) {
    char temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

// Recursive function to print permutations
void permute(char *a, int l, int r) {
    int i;
    if (l == r) {
        printf("%s  ", a);
    } else {
        for (i = l; i <= r; i++) {
            swap((a + l), (a + i));
            permute(a, l + 1, r);
            swap((a + l), (a + i)); // Backtrack
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[MAXSIZE];
    socklen_t addr_size;
    int n;

    // 1. Create UDP Socket (SOCK_DGRAM)
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 2. Bind
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    printf("[UDP Server] Waiting for data...\n");

    while (1) {
        addr_size = sizeof(clientAddr);
        memset(buffer, 0, MAXSIZE);

        // 3. Receive message from Client
        n = recvfrom(sockfd, buffer, MAXSIZE, 0, (struct sockaddr*)&clientAddr, &addr_size);
        
        if (strcmp(buffer, "exit") == 0) {
            printf("\n[Server] Client sent exit. Stopping.\n");
            break;
        }

        printf("\n[Server] Received string: %s\n", buffer);
        printf("Permutations:\n");
        
        // 4. Calculate and Print Permutations
        int len = strlen(buffer);
        permute(buffer, 0, len - 1);
        printf("\n-----------------------------\n");

        // 5. Send Acknowledgement (Half Duplex flow control)
        char *ack = "Permutations displayed on Server.";
        sendto(sockfd, ack, strlen(ack), 0, (struct sockaddr*)&clientAddr, addr_size);
    }

    close(sockfd);
    return 0;
}