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

	printf("\n\n\nvalores: %s", userMessage.username.content);

	printf("AVAILABLE USERS: \n");
	printf("##################################################\n");
	printContactList();
	printf("##################################################\n");

	bzero(buff, sizeof(buff));
	printf("\nEnter the user : ");
	n = 0;
	while ((buff[n++] = getchar()) != '\n');
	n--;
	bzero(userMessage.userDestiny, 1024);
	strncpy(userMessage.userDestiny, buff, n);	


	//strcpy(userMessage.userDestiny, "hoiVintas");

	//INIT USER
	//read(sockfd, (char *) &serverResp, sizeof(serverResp));

	write(*sockfd, (char *) &userMessage, sizeof(msg));

	read(*sockfd, (struct serverResponse *) &serverResp, sizeof(serverResponse));

	printf("\n\nServer answer: %s", serverResp.message);

	//read(*sockfd, &msg1, sizeof(msg1));
	//printf("\n\nServer answer1: %s", msg1);

	pthread_create(&thread_id, NULL, (void *)recvMsg, sockfd);

	for (;;) {
		bzero(buff, sizeof(buff));
		printf("\n\nEnter the string : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n');
		n--;
		strncpy(userMessage.message.content, buff, n);
		userMessage.message.nBytes = n;

		write(*sockfd, (char *) &userMessage, sizeof(msg));
		bzero(buff, sizeof(buff));
		//strncpy(buff, '\0', sizeof(buff));
		bzero(userMessage.message.content, sizeof(userMessage.message.content));
		//bzero(userMessage.userDestiny, sizeof(userMessage.userDestiny));

		//read(sockfd, buff, sizeof(buff));
		//printf("From Server : %s", buff);

		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}
	}
}


void * recvMsg(void * sockfd){
	int sock = *((int *)sockfd);
	int len;
	//serverResponse serverResp;
	char msg1[1024] = {'\0'};
	//printf("thread client \n");
	//printf("thread client socket: %d \n", sock);

	// client thread always ready to receive message
	

	//recv(sockfd,msg,1024,0);
	//recv(sockfd, &msg, sizeof(msg));
	while(1){
		read(sock, msg1, 1023);
		
		printf("\t \nNova mensagem: %s \n", msg1);

		
	}

	

	
}