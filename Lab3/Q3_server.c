#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<ctype.h>
#include<sys/wait.h>

void digits(char *in, char *out) {
    int d[100], n = 0;

    for(int i = 0; in[i]; i++) {
        if(isdigit(in[i]))
            d[n++] = in[i] - '0';
    }

    for(int i = 0; i < n-1; i++)
        for(int j = i+1; j < n; j++)
            if(d[i] > d[j]) {
                int t = d[i]; d[i] = d[j]; d[j] = t;
            }

    int k = 0;
    for(int i = 0; i < n; i++)
        out[k++] = d[i] + '0';

    out[k] = '\0';
}

void chars(char *in, char *out) {
    char c[100];
    int n = 0;

    for(int i = 0; in[i]; i++) {
        if(isalpha(in[i]))
            c[n++] = in[i];
    }

    for(int i = 0; i < n-1; i++)
        for(int j = i+1; j < n; j++)
            if(c[i] < c[j]) {
                char t = c[i]; c[i] = c[j]; c[j] = t;
            }

    c[n] = '\0';
    strcpy(out, c);
}

int main() {
    int port_num;
    printf("Enter the port number: ");
    scanf("%d", &port_num);

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if(s == -1) {
        perror("Socket");
        return 0;
    }

    struct sockaddr_in server, client;
    socklen_t clen = sizeof(client);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port_num);

    if(bind(s, (struct sockaddr*)&server, sizeof(server)) == -1){
        perror("Bind");
        return 0;
    }

    listen(s, 1);
    printf("Waiting for client...\n");

    int ack = accept(s, (struct sockaddr*)&client, &clen);
    printf("Client connected\n");

    char buff[1024];

    while(1) {
        int n = recv(ack, buff, sizeof(buff)-1, 0);
        if(n <= 0) break;

        buff[n] = '\0';

        if(strcmp(buff, "exit") == 0)
            break;

        int pid = fork();

        if(pid == 0) {   // Child → digits
            char result[256], sendbuff[512];
            digits(buff, result);
            sprintf(sendbuff, "Child PID: %d | Digits Ascending: %s",
                    getpid(), result);
            send(ack, sendbuff, strlen(sendbuff), 0);
            exit(0);
        }
        else {          // Parent → characters
            char result[256], sendbuff[512];
            chars(buff, result);
            sprintf(sendbuff, "Parent PID: %d | Characters Descending: %s",
                    getpid(), result);
            send(ack, sendbuff, strlen(sendbuff), 0);
            wait(NULL);   // wait for child
        }
    }

    close(ack);
    close(s);
    return 0;
}