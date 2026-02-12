#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<math.h>

int main()
{
    int sockfd, newsockfd, retval;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;
    int data[7], c1, c2, c3, c, i;

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

    printf("Hamming Code Server Ready...\n");

    while(1)
    {
        actuallen = sizeof(clientaddr);
        newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);

        if(newsockfd == -1)
        {
            close(sockfd);
            continue;
        }

        // Receive the 7-bit codeword
        recv(newsockfd, data, sizeof(data), 0);

        printf("\nReceived Data: ");
        for(i = 0; i < 7; i++)
            printf("%d ", data[i]);
        
        // Calculate Syndrome Bits
        // c1 checks positions 1, 3, 5, 7 (Indices 6, 4, 2, 0 because array is 0-6)
        // Note: Standard Hamming uses 1-based indexing.
        // Let's assume data[0] is D7, data[6] is D1 (LSB).
        // For simplicity in code, we often use:
        // data[6]=P1, data[5]=P2, data[4]=D3, data[3]=P4, data[2]=D5, data[1]=D6, data[0]=D7
        
        // Using standard logic where:
        // c1 = data[6]^data[4]^data[2]^data[0]
        // c2 = data[5]^data[4]^data[1]^data[0]
        // c3 = data[3]^data[2]^data[1]^data[0]
        
        c1 = data[6] ^ data[4] ^ data[2] ^ data[0];
        c2 = data[5] ^ data[4] ^ data[1] ^ data[0];
        c3 = data[3] ^ data[2] ^ data[1] ^ data[0];
        
        c = c3 * 4 + c2 * 2 + c1 * 1;

        if(c == 0)
        {
            printf("\nResult: No Error Detected.\n");
        }
        else
        {
            printf("\nError detected at position: %d\n", c);
            
            // Correct the error (flip the bit)
            // Array index is (7 - c)
            if(data[7-c] == 0)
                data[7-c] = 1;
            else
                data[7-c] = 0;
                
            printf("Corrected Data: ");
            for(i = 0; i < 7; i++)
                printf("%d ", data[i]);
            printf("\n");
        }

        close(newsockfd);
    }
    close(sockfd);
}