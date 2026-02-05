#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>

int count_occ(char *fname, char *word){
    FILE *f = fopen(fname,"r");
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

    struct sockaddr_in server,client;

    int s=socket(AF_INET,SOCK_STREAM,0);
    if(s==-1){
        printf("\nSocket creation error\n");
        return 0;
    }
    printf("\nSocket Created Successfully\n");

    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(port_num);
    server.sin_family=AF_INET;

    if(bind(s, (struct sockaddr*)&server, sizeof(server))==-1){
        printf("\nBinding failed\n");
        close(s);
        return 0;
    }
    printf("\nBind Successful\n");

    if(listen(s,1)==-1){
        close(s);
        return 0;
    }
    printf("\nServer is Listening\n");

    socklen_t clen=sizeof(client);
    int acp=accept(s, (struct sockaddr*)&client, &clen);
    if(acp==-1){
        close(s);
        return 0;
    }
    printf("\nClient Connected Successfully\n");

    char buff[50];
    recv(acp,buff,sizeof(buff),0);

    FILE *f= fopen(buff,"r");
    if(!f){
        printf("\nFile not present\nTerminating Connection\n");
        char temp[]="File not present";
        send(acp, temp, strlen(temp)+1, 0);
        close(acp);
        close(s);
        return 0;
    }
    printf("\nFile %s open\n", buff);
    send(acp, "File found", 11 , 0);

    while(1){
        char opt[10];
        recv(acp, opt, sizeof(opt), 0);

        if(strcmp(opt,"1")==0){
            char word[50];
            recv(acp, word, sizeof(word), 0);
            printf("\nTarget received: %s\n",word);
            int c = count_occ(buff, word);
            char reply[50];
            if(c>0) sprintf(reply,"Found %d times",c);
            else strcpy(reply,"String not found");
            send(acp, reply, strlen(reply)+1, 0);
        }

        else if(strcmp(opt,"2")==0){
            char s1[50], s2[50];
            recv(acp, s1, sizeof(s1), 0);
            printf("\nOld String received: %s\n",s1);
            recv(acp, s2, sizeof(s2), 0);
            printf("\nNew String received to replace old string: %s\n",s1);

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
                send(acp,"String replaced",16,0);
            }else{
                send(acp,"String not found",17,0);
            }
        }

        else if(strcmp(opt,"3")==0){
            FILE *fp = fopen(buff,"r");
            char data[5000], ch;
            printf("\nReordering the file\n");
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

            send(acp,"File reordered",15,0);
        }

        else if(strcmp(opt,"4")==0){
            printf("\nExiting...\n");
            break;
        }
    }

    close(acp);
    close(s);
}