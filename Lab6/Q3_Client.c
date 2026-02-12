#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main()
{
    int sockfd, retval, i;
    struct sockaddr_in serveraddr;
    int data[7], input[4];

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

    printf("Enter 4 bits of data one by one (e.g., 1 0 1 1):\n");
    scanf("%d %d %d %d", &data[0], &data[1], &data[2], &data[4]);

    // Construct Hamming Code (7,4)
    // P1 (at index 6) = D3(4)^D5(2)^D7(0)
    // P2 (at index 5) = D3(4)^D6(1)^D7(0)
    // P4 (at index 3) = D5(2)^D6(1)^D7(0)
    
    // Assigning positions:
    // data[0]=D7, data[1]=D6, data[2]=D5, data[3]=P4, data[4]=D3, data[5]=P2, data[6]=P1
    
    data[6] = data[0] ^ data[2] ^ data[4];
    data[5] = data[0] ^ data[1] ^ data[4];
    data[3] = data[0] ^ data[1] ^ data[2];

    printf("Encoded Data (Hamming 7,4): ");
    for(i = 0; i < 7; i++)
        printf("%d ", data[i]);

    // Error Injection
    int choice, pos;
    printf("\n\nDo you want to inject an error? (1-Yes, 0-No): ");
    scanf("%d", &choice);

    if(choice == 1)
    {
        printf("Enter position to corrupt (1-7): ");
        scanf("%d", &pos);
        // Flip the bit at index (7-pos)
        if(data[7-pos] == 0) data[7-pos] = 1;
        else data[7-pos] = 0;
        
        printf("Transmitting Corrupted Data: ");
        for(i = 0; i < 7; i++)
            printf("%d ", data[i]);
    }

    send(sockfd, data, sizeof(data), 0);
    printf("\nData Sent.\n");

    close(sockfd);
    return 0;
}