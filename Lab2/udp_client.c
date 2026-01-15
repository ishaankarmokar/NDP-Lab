#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#define MAXSIZE 2048

void main()
{
	int sockfd,retval,i, choice;
	struct sockaddr_in serveraddr, clientaddr;
	char buff[MAXSIZE];
    char str1[50], str2[50];

	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd==-1) {
		printf("\nSocket Creation Error");
		return;
	}

    // Client Bind (As per your reference file requirement)
	clientaddr.sin_family=AF_INET;
	clientaddr.sin_port=htons(3389); // Reference uses 3389
	clientaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	
    retval=bind(sockfd,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
	if(retval==1) {
		printf("Binding error");
		close(sockfd);
	}

    // Server Info
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(3388);
	serveraddr.sin_addr.s_addr=inet_addr("127.0.0.1");

    // 1. Send Filename
    printf("Enter filename: ");
    scanf("%s", buff);
    sendto(sockfd,buff,sizeof(buff),0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));

    // 2. Receive Status
    int size=sizeof(serveraddr);
    memset(buff, '\0', sizeof(buff));
    recvfrom(sockfd,buff,sizeof(buff),0,(struct sockaddr*)&serveraddr,&size);
    printf("Server: %s\n", buff);

    if(strcmp(buff, "File not present") == 0) {
        close(sockfd);
        return;
    }

	for (i = 0; ; i+= 1)
	{
        printf("\n1. Search\n2. Replace\n3. Reorder\n4. Exit\nEnter Choice: ");
        scanf("%d", &choice);
        
        memset(buff, '\0', sizeof(buff));

        if (choice == 1) {
            printf("Enter string to search: ");
            scanf("%s", str1);
            sprintf(buff, "1 %s", str1);
        }
        else if (choice == 2) {
            printf("Enter string to find: ");
            scanf("%s", str1);
            printf("Enter string to replace: ");
            scanf("%s", str2);
            sprintf(buff, "2 %s %s", str1, str2);
        }
        else if (choice == 3) {
            sprintf(buff, "3");
        }
        else if (choice == 4) {
            sprintf(buff, "4");
            sendto(sockfd,buff,sizeof(buff),0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
            break;
        }

		sendto(sockfd,buff,sizeof(buff),0,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
		
        memset(buff, '\0', sizeof(buff));
		recvfrom(sockfd,buff,sizeof(buff),0,(struct sockaddr*)&serveraddr,&size);
		puts(buff);
	}
	close(sockfd);
}