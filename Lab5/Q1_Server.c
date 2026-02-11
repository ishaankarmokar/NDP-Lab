#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define MAXSIZE 100

int main()
{
    int sockfd, newsockfd, retval, i;
    socklen_t actuallen;
    struct sockaddr_in serveraddr, clientaddr;
    char buff[MAXSIZE];
    int client_count = 0;
    int client_sockets[10]; // To store socket IDs for termination

    // Initialize the file with "Manipal"
    FILE *fp = fopen("data.txt", "w");
    fprintf(fp, "Manipal");
    fclose(fp);

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

    printf("Server is ready (Concurrent)\n");

    while(1)
    {
        actuallen = sizeof(clientaddr);
        newsockfd = accept(sockfd, (struct sockaddr*)&clientaddr, &actuallen);

        if(newsockfd == -1)
        {
            close(sockfd);
            continue;
        }

        // Add new socket to array and increment count
        client_sockets[client_count] = newsockfd;
        client_count++;

        if(client_count > 2)
        {
            printf("\nMax clients exceeded. Terminating all sessions.\n");
            // Send terminate message to all tracked clients
            for(i = 0; i < client_count; i++)
            {
                send(client_sockets[i], "terminate session", 18, 0);
                close(client_sockets[i]);
            }
            close(sockfd);
            remove("data.txt"); // Clean up
            return 0;
        }

        int pid = fork();

        if(pid == 0)
        {
            // Child Process
            close(sockfd); // Child doesn't need the listener

            memset(buff, '\0', sizeof(buff));
            recv(newsockfd, buff, sizeof(buff), 0);
            
            // Append to file
            fp = fopen("data.txt", "a");
            fprintf(fp, " %s", buff);
            fclose(fp);

            // Read back the full string to display
            fp = fopen("data.txt", "r");
            char file_content[200];
            fgets(file_content, 200, fp);
            fclose(fp);

            printf("\nClient Address: %s : %d", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
            printf("\nCurrent File Content: %s\n", file_content);

            close(newsockfd);
            exit(0);
        }
        else
        {
            // Parent Process
            // Note: In a real persistent chat, we wouldn't close newsockfd here immediately
            // if we wanted to send the "terminate" message later. 
            // But for this lab logic where clients send once and exit, we keep going.
        }
    }
    close(sockfd);
}