#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <ctype.h>

#define PORT 3388
#define MAXSIZE 1024

// Helper to sort a string in Ascending order
void sort_ascending(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len - 1; i++) {
        for (int j = 0; j < len - i - 1; j++) {
            if (str[j] > str[j + 1]) {
                char temp = str[j];
                str[j] = str[j + 1];
                str[j + 1] = temp;
            }
        }
    }
}

// Helper to sort a string in Descending order
void sort_descending(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len - 1; i++) {
        for (int j = 0; j < len - i - 1; j++) {
            if (str[j] < str[j + 1]) {
                char temp = str[j];
                str[j] = str[j + 1];
                str[j + 1] = temp;
            }
        }
    }
}

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;
    char buffer[MAXSIZE];
    char tempBuffer[MAXSIZE];
    char resultBuffer[MAXSIZE];
    pid_t pid;

    // 1. Create Socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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

    // 3. Listen
    if (listen(sockfd, 5) == 0) {
        printf("Server listening on port %d...\n", PORT);
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

    // 5. Receive String
    memset(buffer, 0, MAXSIZE);
    recv(newsockfd, buffer, MAXSIZE, 0);
    printf("Received raw string: %s\n", buffer);

    // 6. Fork
    pid = fork();

    if (pid == 0) {
        // --- CHILD PROCESS: Sort NUMBERS Ascending ---
        int j = 0;
        memset(tempBuffer, 0, MAXSIZE);
        
        // Extract digits
        for (int i = 0; i < strlen(buffer); i++) {
            if (isdigit(buffer[i])) {
                tempBuffer[j++] = buffer[i];
            }
        }
        tempBuffer[j] = '\0';

        // Sort Ascending
        sort_ascending(tempBuffer);

        // Prepare response
        memset(resultBuffer, 0, MAXSIZE);
        sprintf(resultBuffer, "Output at Child (PID %d): %s", getpid(), tempBuffer);
        
        // Send
        send(newsockfd, resultBuffer, strlen(resultBuffer), 0);
        close(newsockfd);
        exit(0);
    } 
    else {
        // --- PARENT PROCESS: Sort CHARACTERS Descending ---
        // Sleep briefly to ensure Child processes distinct send (optional but helps prevents packet merging in labs)
        sleep(1); 
        
        int j = 0;
        memset(tempBuffer, 0, MAXSIZE);

        // Extract alphabets
        for (int i = 0; i < strlen(buffer); i++) {
            if (isalpha(buffer[i])) {
                tempBuffer[j++] = buffer[i];
            }
        }
        tempBuffer[j] = '\0';

        // Sort Descending
        sort_descending(tempBuffer);

        // Prepare response
        memset(resultBuffer, 0, MAXSIZE);
        sprintf(resultBuffer, "Output at Parent (PID %d): %s", getpid(), tempBuffer);

        // Send
        send(newsockfd, resultBuffer, strlen(resultBuffer), 0);
        
        // Parent waits for child to finish to avoid zombie process (optional in simple server)
        wait(NULL); 
        close(newsockfd);
        close(sockfd);
    }

    return 0;
}