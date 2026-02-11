#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<time.h>

#define MAXSIZE 100

// Helper function to sort a string for anagram checking
void sort_string(char *str) {
    char temp;
    int i, j;
    int n = strlen(str);
    for (i = 0; i < n-1; i++) {
        for (j = i+1; j < n; j++) {
            if (str[i] > str[j]) {
                temp = str[i];
                str[i] = str[j];
                str[j] = temp;
            }
        }
    }
}

int main()
{
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;
    char str1[MAXSIZE], str2[MAXSIZE];
    char buffer[MAXSIZE];
    time_t t;

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

    printf("Iterative Server Ready (Waiting for clients)...\n");

    while(1)
    {
        actuallen = sizeof(clientaddr);
        // Accept waits for a connection (Blocking)
        newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);

        if(newsockfd == -1)
        {
            close(sockfd);
            continue;
        }

        // 1. Display Date/Time and Client Address
        time(&t);
        printf("\nConnected to Client: %s:%d\nTime: %s", 
               inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), ctime(&t));

        // 2. Receive the two strings
        memset(str1, 0, sizeof(str1));
        memset(str2, 0, sizeof(str2));
        
        recv(newsockfd, str1, sizeof(str1), 0);
        recv(newsockfd, str2, sizeof(str2), 0);

        printf("Received strings: %s, %s\n", str1, str2);

        // 3. Check for Anagrams
        if(strlen(str1) != strlen(str2)) {
            strcpy(buffer, "Not Anagrams (Length mismatch)");
        } else {
            sort_string(str1);
            sort_string(str2);
            
            if(strcmp(str1, str2) == 0)
                strcpy(buffer, "Strings are Anagrams");
            else
                strcpy(buffer, "Strings are NOT Anagrams");
        }

        // 4. Send Result
        send(newsockfd, buffer, sizeof(buffer), 0);

        // 5. Close connection (Iterative nature)
        close(newsockfd);
    }
    close(sockfd);
}