#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "lib/structs.h"
#include "lib/filesOps.h"
#include <pthread.h>

#define MAX 1024
#define PORT 1234

#define SA struct sockaddr
pthread_mutex_t mutex;


void messageHandler(int * sockfd);
void * recvMsg(void * sockfd);

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
	servaddr.sin_addr.s_addr =  htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat
	messageHandler(&sockfd);

	// close the socket
	close(sockfd);
}

void messageHandler(int *sockfd){
	char buff[MAX]={0};
	int n;
	msg userMessage;
	serverResponse serverResp;
	pthread_t thread_id;
	char msg1[1024];
	
	printf("Your username : ");
	n = 0;
	while ((buff[n++] = getchar()) != '\n');
	n--;
	printf("\n");
	__fpurge(stdin);

	strncpy(userMessage.username.content, buff, n);
	userMessage.username.nBytes = n;

	write(*sockfd, (char *) &userMessage, sizeof(msg));

	//server response:
	read(*sockfd, (serverResponse *) &serverResp, sizeof(serverResponse));
	if(serverResp.operation == 1) {

		if(hasMessage(userMessage.username.content) == 1)
			printf("\n\nYou have an unread message: %s\n\n", serverResp.payload.message.content);
		else 
			printf("\n\nServer answer: %s\n\n", serverResp.payload.message.content);
	}else {
		printf("\n Server not responding ...\n");
		close(*sockfd);
		exit(0);
	}


	//THREAD TO RECEIVE INCOMING MESSAGES
	pthread_create(&thread_id, NULL, (void *)recvMsg, sockfd);
	//END

	for (;;) {

		//send message to
		bzero(buff, sizeof(buff));
		bzero(&userMessage, sizeof(userMessage));
		printf("\nEnter the user you want to message: ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n');
		n--;
		
		//bzero(userMessage.userDestiny.content, 1024);
		strncpy(userMessage.userDestiny.content, buff, n);	
		userMessage.userDestiny.nBytes = n;
		userMessage.operation = 1;
		//end

		//message to be send
		bzero(buff, sizeof(buff));
		printf("\n\nEnter the string : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n');
		n--;
		strncpy(userMessage.message.content, buff, n);
		userMessage.message.nBytes = n;
		//end

		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");

			userMessage.operation = 4; //exit operation
			write(*sockfd, (char *) &userMessage, sizeof(msg));
			close(*sockfd);
			break;
		}

		write(*sockfd, (char *) &userMessage, sizeof(msg));
	}
}


void * recvMsg(void * sockfd){
	int sock = *((int *)sockfd);
	int len;
	msg msgFromServer;
	serverResponse server;

	while(1) {
		read(sock, (serverResponse *) &server, sizeof(serverResponse));
		switch(server.operation){
			case 1:
				printf("\t \nServer close connection. Exiting..... \n");	
				close(sock);
				exit(0);

			break;

			case 2:
				printf("\t \nReceived message from: %s \n", server.payload.userDestiny.content);	
				printf("\t \nMessage: %s \n", server.payload.message.content);	

			break;

			case 3: //CONTACT LIST


			break;

			case 4: //INCOMING FILE
	

			break;
		}
	}
}