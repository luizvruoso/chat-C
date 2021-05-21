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
void sendFile(int , msg );
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
	int  op;
	
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
	if(serverResp.operation == 1){
		printf("\n\nServer answer: %s", serverResp.payload.message.content);
	}else{
		printf("\n Server not responding ...\n");
		close(*sockfd);
		exit(0);
	}


	//THREAD TO RECIVE INCOMING MESSAGES
	pthread_create(&thread_id, NULL, (void *)recvMsg, sockfd);
	//END

	for (;;) {
		menu(&op);

		switch(op){
			case 1: 
				sendUserMessage(*sockfd, userMessage);
			break;
			case 2:
				sendFile(*sockfd, userMessage);
			break;

		}
		
	}
}



void sendFile(int sock, msg userMessage){
	serverResponse server;

	//SINALIZANDO PARA O SERVIDOR O ENVIO DE UM ARQUIVO
	userMessage.operation = 2;
	printf("user %d\n\n", userMessage.operation);
	write(sock, (char *) &userMessage, sizeof(msg));
	//FIM
	

	char data[MAX] = {0};
	FILE *filePointer;

	

	filePointer = fopen("file.txt", "r");

	

	while(!feof(filePointer)) {
		fgets(server.payload.message.content, sizeof(server.payload.message.content), filePointer);
		printf("get\n\n");
		if (write(sock, (char *) &server, sizeof(serverResponse)) == -1) {
			perror("error in sending data");
			break;
			//exit(1);
		}
		printf("enviou\n\n");

		//bzero(data, MAX);
	}
	printf("Cheguei no final \n\n");

	fclose(filePointer);
	server.operation = -1;
	write(sock, (char *) &server, sizeof(serverResponse));
	return ;



}


void sendUserMessage(int sock, msg userMessage){
		char buff[MAX]={0};
		int n;
		//send message to
		bzero(buff, sizeof(buff));
		printf("\nEnter the user : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n');
		n--;
		bzero(userMessage.userDestiny.content, 1024);
		strncpy(userMessage.userDestiny.content, buff, n);	
		userMessage.userDestiny.nBytes = n;
		
		
		userMessage.operation = 4;// send file operation
		//end
		
		//message to be sended
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
			write(sock, (char *) &userMessage, sizeof(msg));
			close(sock);
			exit(1);
		}

		

		write(sock, (char *) &userMessage, sizeof(msg));

		// envio 
}


void menu(int * op){

	printf("\n1 - Send Message \n");
	printf("2 - Send File \n");
	printf("3 - Exit \n");

	scanf("%d", op);
	getchar();
	
}


void * recvMsg(void * sockfd){
	int sock = *((int *)sockfd);
	int len;
	msg msgFromServer;
	serverResponse server;

	while(1){
		read(sock, (serverResponse *) &server, sizeof(serverResponse));
		switch(server.operation){
			case 1:
				printf("\t \nServer close connection. Exiting..... \n");	
				close(sock);
				exit(0);

			break;

			case 2:
				printf("\t \nRecieved message from: %s \n", server.payload.userDestiny.content);	
				printf("\t \nMessage: %s \n", server.payload.message.content);	

			break;

			case 3: //CONTACT LIST


			break;

			case 4: //INCOMING FILE
	

			break;
		}

		
		
	}

	

	
}