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

    int s = socket(AF_INET, SOCK_STREAM, 0);
    if(s == -1){
        printf("\nSocket creation error\n");
        return 0;
    }
    printf("\nSocket Created Successfully\n");

    server.sin_family = AF_INET;
    server.sin_port = htons(port_num);
    server.sin_addr.s_addr = INADDR_ANY;

    if(bind(s, (struct sockaddr*)&server, sizeof(server)) == -1){
        printf("\nBinding failed\n");
        close(s);
        return 0;
    }
    
    if(listen(s, 5) == -1){
        printf("\nListen failed\n");
        close(s);
        return 0;
    }
    printf("\nTCP Server Ready...\n");

    while(1){
        int acp = accept(s, (struct sockaddr*)&client, &clen);
        if(acp == -1){
            printf("Connection error\n");
            continue;
        }

        // Parent receives the option to decide which child to fork
        int option;
        recv(acp, &option, sizeof(option), 0);
        
        char input[50];
        recv(acp, input, sizeof(input), 0);

        int pid = fork();

        if(pid == 0){
            // Child Process
            close(s); // Child doesn't need listener
            char reply[200];
            int my_pid = getpid();

            if(option == 1){
                // Child 1: Name and Address
                // Logic based on Reg No (input)
                sprintf(reply, "PID: %d | Name: Ishaan, Address: Manipal, Karnataka", my_pid);
            }
            else if(option == 2){
                // Child 2: Enrollment Details
                // Logic based on Student Name (input)
                sprintf(reply, "PID: %d | Dept: CCE, Sem: 6, Sec: A, Courses: OS, Networks", my_pid);
            }
            else if(option == 3){
                // Child 3: Marks
                // Logic based on Subject Code (input)
                sprintf(reply, "PID: %d | Subject: %s, Marks Obtained: 92", my_pid, input);
            }
            else {
                sprintf(reply, "PID: %d | Invalid Option", my_pid);
            }

            send(acp, reply, sizeof(reply), 0);
            close(acp);
            exit(0);
        }
        else {
            // Parent Process
            close(acp); // Parent closes connected socket
        }
    }
    close(s);
    return 0;
}