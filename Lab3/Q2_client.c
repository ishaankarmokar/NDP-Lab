#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    int port_num;
    char buffer[1024];

    printf("Enter port number: ");
    scanf("%d", &port_num);

    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    struct sockaddr_in server;
    socklen_t slen = sizeof(server);

    server.sin_family = AF_INET;
    server.sin_port = htons(port_num);
    server.sin_addr.s_addr =inet_addr("127.0.0.1");

    printf("Enter string to permute: ");
    scanf("%s", buffer);

    sendto(s, buffer, strlen(buffer), 0,
           (struct sockaddr *)&server, slen);

    memset(buffer, 0, sizeof(buffer));
    recvfrom(s, buffer, sizeof(buffer), 0,
             (struct sockaddr *)&server, &slen);

    printf("Server reply: %s\n", buffer);

    close(s);
    return 0;
}