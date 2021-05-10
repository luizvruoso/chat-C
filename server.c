#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "lib/structs.h"

#define MAX 80
#define PORT 8080

#define SA struct sockaddr

// Function designed for chat between client and server.
void func(int sockfd);

// Driver function
int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");

	len = sizeof(cli);

	// Accept the data packet from client and verification
	for(int i = 0; i < 5; i++){

		if(fork() == 0)
			do{
				connfd = accept(sockfd, (SA*)&cli, &len);
				if (connfd < 0) {
					printf("server acccept failed...\n");
					exit(0);
				}
				else
					printf("server accept the client...\n");

					printf("\n\nHoi?");

				// Function for chatting between client and server
				//printf("\nDescritores:  sock: %d    msgsock:%d \n\n", sockfd, connfd);
				func(connfd);

				// After chatting close the socket
				close(sockfd);
			}while(1);
	}

	for(int i = 0; i < 5; i++){
		wait();
	}

}


void func(int sockfd)
{
	char buff[MAX];
	int n;
	serverResponse message;
	msg userMessage;
	FILE *file;
	file = fopen ("bd.txt", "w+");
	

	read(sockfd, (char *) &userMessage, sizeof(userMessage));
	
	strcpy(message.message, "YES u can!");
	fprintf(file, "%s",userMessage.username);
	fprintf(file,"ONLINE");
	
	fclose(file);
	
	//strcpy(message.statusCode, "200");
	
	write(sockfd, (char *) &message, sizeof(message));

	// infinite loop for chat
	while(1) {
		bzero(buff, MAX);

		// read the message from client and copy it in buffer
		read(sockfd, buff, sizeof(buff));
		// print buffer which contains the client contents
		printf("\nFrom client: %s\t To client : ", buff);
		printf("\n");
		bzero(buff, MAX);
		n = 0;
		// copy server message in the buffer
		
		//while ((buff[n++] = getchar()) != '\n');

		// and send that buffer to client

		//write(sockfd, buff, sizeof(buff));

		// if msg contains "Exit" then server exit and chat ended.
		if (strncmp("exit", buff, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}

		exit(0);
	}
}