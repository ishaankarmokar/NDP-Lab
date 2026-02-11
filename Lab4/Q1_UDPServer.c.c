#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main(){
    int port_num;
    printf("Enter port number: ");
    scanf("%d",&port_num);

    struct sockaddr_in server, client;
    socklen_t clen = sizeof(client);

    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if(s == -1){
        printf("Socket creation error\n");
        return 0;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port_num);
    server.sin_addr.s_addr = INADDR_ANY;

    if(bind(s, (struct sockaddr*)&server, sizeof(server)) == -1){
        printf("Bind failed\n");
        close(s);
        return 0;
    }

    printf("\nUDP Server Ready...\n");

    while(1){
        int option;
        char input[50];
        
        // Receive Option
        recvfrom(s, &option, sizeof(option), 0, (struct sockaddr*)&client, &clen);
        // Receive Details
        recvfrom(s, input, sizeof(input), 0, (struct sockaddr*)&client, &clen);

        int pid = fork();

        if(pid == 0){
            // Child Process
            char reply[200];
            int my_pid = getpid();

            if(option == 1){
                // Child 1
                sprintf(reply, "PID: %d | Name: Ishaan, Address: Manipal", my_pid);
            }
            else if(option == 2){
                // Child 2
                sprintf(reply, "PID: %d | Dept: CCE, Sem: 6, Courses: 5", my_pid);
            }
            else if(option == 3){
                // Child 3
                sprintf(reply, "PID: %d | Subject: %s, Marks: 88", my_pid, input);
            }

            // Send to the client address captured by parent
            sendto(s, reply, sizeof(reply), 0, (struct sockaddr*)&client, clen);
            exit(0);
        }
    }
    close(s);
    return 0;
}