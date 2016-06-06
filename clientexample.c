#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

char buffer[250];



int main(int argc, char *argv[])
{

    fd_set read_fd_set;
    int sockfd = 0, portnum = atoi(argv[2]);
    char message[100];


    struct sockaddr_in serv_addr; 
    struct hostent *server;

    memset(message, '\0',sizeof(message));
    
    if(argc < 3){ printf("not enough argv\n");}
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Socket Error \n");
    } 
    
    server = gethostbyname(argv[1]);
    if(server == NULL){printf("server error\n");}

    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portnum); 
    
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("Connection error\n");
    } 

    while(1){	
	FD_ZERO(&read_fd_set);
	FD_SET(3,&read_fd_set);
	FD_SET(0,&read_fd_set);

	select(4,&read_fd_set,NULL,NULL,NULL);
	if(FD_ISSET(0,&read_fd_set)){
        printf("What message would you like to send?\n");
        fgets(message, 250, stdin);
	message[strlen(message) - 1] = '\0';
        write(sockfd, message, strlen(message)); //need to erorrcheck write
        memset(message, '\0', sizeof(message));
	}
	else{
	//read from client
	read(sockfd,buffer,250);
	printf("client: got message: '%s' \n", buffer);
	}
        
   }

    return 0;
}
