#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>

int count_occ(char *fname, char *word){
    FILE *f = fopen(fname,"r");
    if(!f) return -1;

    char line[500];
    int count = 0;
    while(fgets(line,500,f)){
        if(strstr(line, word))
            count++;
    }
    fclose(f);
    return count;
}

int main(){
    int port_num;
    printf("Enter port number: ");
    scanf("%d",&port_num);

    struct sockaddr_in server, client;
    socklen_t clen = sizeof(client);

    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if(s == -1){
        printf("Socket error\n");
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

    printf("UDP Server Ready...\n");

    char buff[50];
    recvfrom(s, buff, sizeof(buff), 0, (struct sockaddr*)&client, &clen);

    FILE *f = fopen(buff,"r");
    if(!f){
        char temp[]="File not present";
        sendto(s, temp, strlen(temp)+1, 0, (struct sockaddr*)&client, clen);
        close(s);
        return 0;
    }

    printf("File %s opened\n", buff);
    sendto(s, "File found", 11, 0, (struct sockaddr*)&client, clen);

    while(1){
        char opt[10];
        recvfrom(s, opt, sizeof(opt), 0, (struct sockaddr*)&client, &clen);

        if(strcmp(opt,"1")==0){
            char word[50];
            recvfrom(s, word, sizeof(word), 0, (struct sockaddr*)&client, &clen);

            int c = count_occ(buff, word);
            char reply[50];

            if(c>0) sprintf(reply,"Found %d times",c);
            else strcpy(reply,"String not found");

            sendto(s, reply, strlen(reply)+1, 0, (struct sockaddr*)&client, clen);
        }

        else if(strcmp(opt,"2")==0){
            char s1[50], s2[50];
            recvfrom(s, s1, sizeof(s1), 0, (struct sockaddr*)&client, &clen);
            recvfrom(s, s2, sizeof(s2), 0, (struct sockaddr*)&client, &clen);

            FILE *fp = fopen(buff,"r");
            char line[500], text[5000]="";
            int found = 0;

            while(fgets(line,500,fp)){
                char *pos = strstr(line, s1);
                if(pos){
                    found = 1;
                    *pos = '\0';
                    strcat(text,line);
                    strcat(text,s2);
                    strcat(text,pos+strlen(s1));
                }else strcat(text,line);
            }
            fclose(fp);

            if(found){
                fp = fopen(buff,"w");
                fputs(text,fp);
                fclose(fp);
                sendto(s,"String replaced",16,0,(struct sockaddr*)&client,clen);
            }else{
                sendto(s,"String not found",17,0,(struct sockaddr*)&client,clen);
            }
        }

        else if(strcmp(opt,"3")==0){
            FILE *fp = fopen(buff,"r");
            char data[5000], ch;
            int i=0;

            while((ch=fgetc(fp))!=EOF) data[i++]=ch;
            data[i]='\0';
            fclose(fp);

            for(int x=0;x<i;x++)
                for(int y=x+1;y<i;y++)
                    if(data[x]>data[y]){
                        char t=data[x]; data[x]=data[y]; data[y]=t;
                    }

            fp = fopen(buff,"w");
            fputs(data,fp);
            fclose(fp);

            sendto(s,"File reordered",15,0,(struct sockaddr*)&client,clen);
        }

        else if(strcmp(opt,"4")==0){
            printf("Client exited\n");
            break;
        }
    }

    close(s);
}