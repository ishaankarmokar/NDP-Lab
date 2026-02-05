#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

void swap(char *a, char *b) {
    char temp = *a;
    *a = *b;
    *b = temp;
}

void permute(char *str, int l, int r) {
    int i;
    if (l == r) {
        printf("%s\n", str);
    } else {
        for (i = l; i <= r; i++) {
            swap(&str[l], &str[i]);
            permute(str, l + 1, r);
            swap(&str[l], &str[i]);
        }
    }
}

int main() {
    int port_num;
    printf("Enter port number: ");
    scanf("%d", &port_num);

    int s;
    struct sockaddr_in server, client;
    socklen_t clen = sizeof(client);
    char buffer[1024];

    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port_num);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(s, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        close(s);
        exit(1);
    }

    printf("\nUDP Server Ready...\n");

    while (1) {
        memset(buffer, 0, sizeof(buffer));

        recvfrom(s, buffer, sizeof(buffer), 0,
                 (struct sockaddr *)&client, &clen);

        printf("\nReceived string: %s\n", buffer);
        printf("Permutations:\n");

        int n = strlen(buffer);
        permute(buffer, 0, n - 1);

        char ack[] = "Permutations printed on server";
        sendto(s, ack, strlen(ack), 0,(struct sockaddr *)&client, clen);
        break;
    }

    close(s);
    return 0;
}