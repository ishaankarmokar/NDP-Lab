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
    char data[MAXSIZE], frame[MAXSIZE];
    char reply[MAXSIZE];
    int i, ones = 0;

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
        return 0;
    }

    printf("Enter Binary Data (e.g. 10110): ");
    scanf("%s", data);

    // Calculate Parity Bit (Even Parity)
    for(i = 0; i < strlen(data); i++)
    {
        if(data[i] == '1')
            ones++;
    }

    // Append Parity Bit
    strcpy(frame, data);
    if(ones % 2 == 0)
        strcat(frame, "0"); // Already even, append 0
    else
        strcat(frame, "1"); // Odd, append 1 to make it even

    printf("Calculated Frame (Data+Parity): %s\n", frame);

    // Option to simulate error
    int choice;
    printf("Do you want to corrupt the data? (1-Yes, 0-No): ");
    scanf("%d", &choice);

    if(choice == 1)
    {
        int len = strlen(frame);
        // Flip the last bit to simulate corruption
        if(frame[len-1] == '0') frame[len-1] = '1';
        else frame[len-1] = '0';
        printf("Transmitting Corrupt Frame: %s\n", frame);
    }
    else
    {
        printf("Transmitting Correct Frame: %s\n", frame);
    }

    send(sockfd, frame, sizeof(frame), 0);

    memset(reply, '\0', sizeof(reply));
    recv(sockfd, reply, sizeof(reply), 0);
    printf("\nServer Reply: %s\n", reply);

    close(sockfd);
    return 0;
}