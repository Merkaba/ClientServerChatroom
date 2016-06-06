#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define PORT    3338

int clientList[FD_SETSIZE];
int counter = 0;

char ** nameHolder[100];

void broadcastToClients(int filedes, char * buffer){
    //printf("got msg %s\n", buffer);
    char * name = malloc(50);
    char * temp;
    //printf("fd is %d\n", filedes); 
    if(buffer[0] != ' '){ 
        temp = strtok(buffer, " ");
        if(strcmp(temp, "ENTER") == 0){
            temp = strtok(NULL, " ");
            memcpy(name, temp, 50);
            nameHolder[filedes] = name;

            int i;
            for(i = 0; i < counter; i++){
                if(clientList[i] != 0){
                    char holder[200];
                    sprintf(holder, "User %s entered.", nameHolder[filedes]);
                    write(clientList[i], holder, 200);
                }               
            }

            //printf("user %s entered\n", name);
        }
        else if(strcmp(temp, "SAY") == 0){
            int i;
            for(i = 0; i < counter; i++){
                if(clientList[i] != 0){
                    char holder[200];
                    sprintf(holder, "%s: %s", nameHolder[filedes], buffer + 4);
                    write(clientList[i], holder, 200);
                }               
            }

            printf("%s: %s\n", nameHolder[filedes], buffer + 4);
        }
        else if(strcmp(temp, "LEAVE") == 0){
            int i;
            for(i = 0; i < counter; i++){
                if(clientList[i] != 0){
                    char holder[200];
                    sprintf(holder, "User %s is leaving.", nameHolder[filedes]);
                    write(clientList[i], holder, 200);
                }
            }

            for(i = 0; i < counter; i++){
                if(filedes == clientList[i]){
                    clientList[i] = 0;
                    break;
                }
            }
        }
    }
}


int socketCreate(int port)
{
    int sock;
    struct sockaddr_in name;

    sock = socket (PF_INET, SOCK_STREAM, 0);
    if (sock < 0){ printf ("socket error\n"); }

    name.sin_family = AF_INET;
    name.sin_port = htons (port);
    name.sin_addr.s_addr = htonl (INADDR_ANY);
    if (bind(sock,(struct sockaddr *) &name, sizeof(name)) < 0)
    {
      printf("bind messed up\n");
    }
    //printf("made the sock\n");
    return sock;
}


int getMessage(int filedes)
{
    char buffer[100];
    int length;
    memset(buffer, '\0', 100);
    length = read(filedes, buffer, 100);
    //printf("the length of the message was %d\n", length);
    if(length < 0) { printf("got 0 bytes read error\n"); }
    else if (length == 0)
        return -1; //eof
    else{
        broadcastToClients(filedes, buffer);
        return 0;
    }
}

int main()
{
    int count;
    for(count = 0; count < 100; count++){
        nameHolder[count] = malloc(50);
    }

    for(count = 0; count < FD_SETSIZE; count++){
        clientList[counter] = 0;
    }

    int socketCreate(int port);
    int sock, i, size;
    fd_set active_fd, read_fd_set;
    struct sockaddr_in clientname;

    sock = socketCreate(PORT);
    if (listen(sock,1) < 0) { printf("error on listen\n"); }

    FD_ZERO(&active_fd);
    FD_SET(sock, &active_fd);

    while(1){
        read_fd_set = active_fd;
        if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0){
            printf("error on selecting\n");
        }     
        for (i = 0; i < FD_SETSIZE; ++i)
        if (FD_ISSET(i, &read_fd_set)){
            if (i == sock){
                int new;
                size = sizeof(clientname);
                new = accept(sock,(struct sockaddr *) &clientname, &size);
                if (new < 0){ printf("file desc fail for accept??\n");}
                FD_SET(new, &active_fd);
                clientList[counter++] = new;
            }
            else{
                if(getMessage(i) < 0) { close(i); FD_CLR(i, &active_fd); }
            }
        }
    }
}
