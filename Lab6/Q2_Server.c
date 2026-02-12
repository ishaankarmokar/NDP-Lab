#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define MAXSIZE 100

// XOR Logic for CRC Division
void xor_div(char *data, char *poly) {
    int i, j;
    int data_len = strlen(data);
    int poly_len = strlen(poly);
    
    for(i = 0; i < data_len - poly_len + 1; i++) {
        if(data[i] == '1') {
            for(j = 0; j < poly_len; j++) {
                if(data[i+j] == poly[j])
                    data[i+j] = '0';
                else
                    data[i+j] = '1';
            }
        }
    }
}

int main() {
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;
    char data[MAXSIZE], poly[MAXSIZE];
    char temp[MAXSIZE]; // To hold data for division
    int choice;

    // Polynomials
    char crc12[] = "1100000001111";
    char crc16[] = "11000000000000101";
    char crc_ccitt[] = "10001000000100001";

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        printf("\nSocket Creation Error");
        return 0;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = htons(INADDR_ANY);


    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    retval = bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if(retval == -1) {
        printf("Binding error");
        close(sockfd);
        return 0;
    }

    listen(sockfd, 5);
    printf("CRC Server Ready...\n");

    while(1) {
        actuallen = sizeof(clientaddr);
        newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);

        if(newsockfd == -1) {
            close(sockfd);
            continue;
        }

        // 1. Receive Polynomial Choice
        recv(newsockfd, &choice, sizeof(choice), 0);
        
        // 2. Receive Data (Message + CRC)
        memset(data, '\0', sizeof(data));
        recv(newsockfd, data, sizeof(data), 0);

        printf("\nClient Selected Option: %d", choice);
        printf("\nReceived Data Frame: %s", data);

        // Select Polynomial
        if(choice == 1) strcpy(poly, crc12);
        else if(choice == 2) strcpy(poly, crc16);
        else strcpy(poly, crc_ccitt);

        // 3. Perform CRC Check
        strcpy(temp, data); // Make a copy for division
        xor_div(temp, poly);

        // 4. Check if Remainder is all zeros
        int error = 0;
        int len = strlen(temp);
        // The remainder is at the end of the temp string
        // We check the last (poly_len - 1) bits
        for(int i = 0; i < len; i++) {
            if(temp[i] == '1') {
                error = 1;
                break;
            }
        }

        if(error == 0) {
            printf("\nResult: Data is Valid (No Error)\n");
            send(newsockfd, "Data Valid", 11, 0);
        } else {
            printf("\nResult: Data Corrupted (CRC Error)\n");
            send(newsockfd, "Data Corrupted", 15, 0);
        }

        close(newsockfd);
    }
    close(sockfd);
}
