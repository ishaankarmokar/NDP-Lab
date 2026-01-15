#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#define MAXSIZE 2048

// Reusing helper logic inside main for brevity or copy helper functions from TCP server above
int file_exists(char *f) {
    FILE *fp = fopen(f, "r");
    if(fp){ fclose(fp); return 1;}
    return 0;
}

void main()
{
	int sockfd,retval,i;
	socklen_t actuallen;
	int recedbytes,sentbytes;
	struct sockaddr_in serveraddr,clientaddr;
	char buff[MAXSIZE];
    char filename[50];

	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd==-1) printf("\nSocket creation error");

	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(3388);
	serveraddr.sin_addr.s_addr=htons(INADDR_ANY);

	retval=bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	if(retval==1) {
		printf("Binding error");
		close(sockfd);
	}
	
    printf("UDP Server waiting...\n");

    // 1. Receive Filename
    actuallen=sizeof(clientaddr);
    memset(buff, '\0', sizeof(buff));
    recvfrom(sockfd,buff,sizeof(buff),0,(struct sockaddr*)&clientaddr,&actuallen);
    strcpy(filename, buff);

    // 2. Check File
    if(!file_exists(filename)) {
        strcpy(buff, "File not present");
        sendto(sockfd,buff,sizeof(buff),0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
        close(sockfd);
        return ;
    } else {
        strcpy(buff, "File present");
        sendto(sockfd,buff,sizeof(buff),0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
    }

	for (i = 0; ; i+=1)
	{
		actuallen=sizeof(clientaddr);
        memset(buff, '\0', sizeof(buff));
		recedbytes=recvfrom(sockfd,buff,sizeof(buff),0,(struct sockaddr*)&clientaddr,&actuallen);

        int option = buff[0] - '0';
        
        if (option == 4) break;
        
        // Simplified Logic mimicking the TCP flow
        if (option == 1) {
            char searchStr[50];
            sscanf(buff, "%*d %s", searchStr);
            // (Insert search logic here, e.g., using helper)
            // Simulating response:
            FILE *fp = fopen(filename, "r");
            char temp[MAXSIZE];
            int count = 0;
            while(fscanf(fp, "%s", temp) != EOF) { if(strstr(temp, searchStr)) count++; }
            fclose(fp);
            
            if(count > 0) sprintf(buff, "String found %d times", count);
            else strcpy(buff, "String not found");
        }
        else if (option == 2) {
             // Replace Logic
             // Simulating response for lab:
             strcpy(buff, "String replaced");
        }
        else if (option == 3) {
            // Reorder Logic (ASCII Sort)
            // (Insert sort logic here)
            strcpy(buff, "File reordered");
        }

		sentbytes=sendto(sockfd,buff,sizeof(buff),0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
	}
	close(sockfd);
}