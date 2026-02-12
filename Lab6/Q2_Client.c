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
    int sockfd, retval, choice;
    struct sockaddr_in serveraddr;
    char input[MAXSIZE], poly[MAXSIZE], frame[MAXSIZE], temp[MAXSIZE];
    char reply[MAXSIZE];

    // Polynomials
    char crc12[] = "1100000001111";       // x^12...
    char crc16[] = "11000000000000101";   // x^16...
    char crc_ccitt[] = "10001000000100001"; // CCITT

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        printf("\nSocket Creation Error");
        return 0;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(3388);
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    retval = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if(retval == -1) {
        printf("Connection error");
        return 0;
    }

    printf("Enter Binary Data (e.g. 1011): ");
    scanf("%s", input);

    printf("\nChoose CRC Polynomial:\n1. CRC-12\n2. CRC-16\n3. CRC-CCITT\nEnter choice: ");
    scanf("%d", &choice);

    if(choice == 1) strcpy(poly, crc12);
    else if(choice == 2) strcpy(poly, crc16);
    else strcpy(poly, crc_ccitt);

    // 1. Prepare Data: Append N-1 zeros (N = poly length)
    strcpy(temp, input);
    int data_len = strlen(input);
    int poly_len = strlen(poly);
    
    for(int i = 0; i < poly_len - 1; i++)
        strcat(temp, "0");

    printf("\nModified Data (Appended Zeros): %s", temp);

    // 2. Perform Division to get Remainder (CRC)
    char div_temp[MAXSIZE];
    strcpy(div_temp, temp); // Work on a copy
    xor_div(div_temp, poly);

    // The 'div_temp' now contains the remainder at the end. 
    // We only care about the last (poly_len - 1) bits.
    char remainder[MAXSIZE];
    strcpy(remainder, &div_temp[data_len]); // Extract remainder

    printf("\nCalculated CRC (Remainder): %s", remainder);

    // 3. Create Final Frame (Data + Remainder)
    strcpy(frame, input);
    strcat(frame, remainder);
    printf("\nFinal Transmitted Frame: %s", frame);

    // 4. Error Simulation
    int err_choice;
    printf("\n\nDo you want to corrupt data? (1-Yes, 0-No): ");
    scanf("%d", &err_choice);
    if(err_choice == 1) {
        // Flip the last bit
        int last = strlen(frame) - 1;
        frame[last] = (frame[last] == '1') ? '0' : '1';
        printf("Corrupted Frame: %s", frame);
    }

    // 5. Send Data
    send(sockfd, &choice, sizeof(choice), 0); // Send choice first
    send(sockfd, frame, sizeof(frame), 0);    // Send frame

    recv(sockfd, reply, sizeof(reply), 0);
    printf("\nServer Reply: %s\n", reply);

    close(sockfd);
    return 0;
}