#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "lib/structs.h"

#define MAX 1024
#define PORT 8080

#define SA struct sockaddr


void messageHandler(int sockfd);

int main()
{
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create and varification
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
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat
	messageHandler(sockfd);

	// close the socket
	close(sockfd);
}

void messageHandler(int sockfd)
{
	char buff[MAX]={0};
	int n;
	msg userMessage;
	serverResponse serverResp;
	printf("Your username : ");
	n = 0;
	while ((buff[n++] = getchar()) != '\n');
	n--;
	printf("\n");
	__fpurge(stdin);

	strncpy(userMessage.username, buff, n);

	//INIT USER
	read(sockfd, (char *) &serverResp, sizeof(serverResp));

	write(sockfd, (char *) &userMessage, sizeof(userMessage));

	read(sockfd, (char *) &serverResp, sizeof(serverResp));

	printf("\n\nServer answer: %s", serverResp.message);

	for (;;) {
		bzero(buff, sizeof(buff));
		printf("\n\nEnter the string : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n');
		write(sockfd, buff, sizeof(buff));
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		printf("From Server : %s", buff);

		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}
	}
}