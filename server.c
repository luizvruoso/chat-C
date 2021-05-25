#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "lib/structs.h"
#include "lib/filesOps.h"
#include <sys/types.h>          /* for wait() */
#include <sys/wait.h>           /* for wait() */
#include <signal.h>             /* for kill(), sigsuspend(), others */
#include <sys/ipc.h>            /* for all IPC function calls */
#include <pthread.h>

#define MAX 1024
#define PORT 2020
#define SA struct sockaddr
#define MAXUSERS 20

int POINTER_LAST_POINTER = 0;

pthread_mutex_t mutex;
user clients[50];

void * registerUser(void *);
void sendToUser(msg * );
void endUserSock(char * );
void recieveFile(int );
void writeFile(int);
void returnUserPeerToPeer(int, msg);


int main() {
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;
	pthread_t thread_id;

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

	for(int i = 0; i < 20; i++) {

		connfd = accept(sockfd, (SA*) &cli, &len);
		if (connfd < 0) {
			printf("server acccept failed...\n");
			exit(0);
		}
		else
			printf("server accept the client...\n");
		
		pthread_mutex_lock(&mutex);

		pthread_create(&thread_id, NULL, (void *)registerUser, &connfd);

		pthread_mutex_unlock(&mutex);

		
	}
	
	return 0;

}

void * registerUser(void * sockfd){
	char buff[MAX];
	int n;
	serverResponse message;
	msg userMessage;
	int sock = *((int *) sockfd);

	read(sock, (struct msg *) &userMessage, sizeof(userMessage));

	printf("User : %s is now connected \n", userMessage.username.content);
	strcpy(message.payload.message.content, "Connected...");
	message.operation = 1;
	message.statusCode = 1; 

	write(sock, (char *) &message, sizeof(serverResponse));

	if(searchUser(userMessage.username.content) == 0) {
		//usuario ja registrado, fica online
		printStatusAtRightPosition(userMessage.username.content, "ONLINE");
	}else {
		//novo usuario é registrado e fica online
		printAtEndOfFile(userMessage.username.content, "ONLINE");
	}

	pthread_mutex_lock(&mutex);
	
		clients[POINTER_LAST_POINTER].socket = &sock;
		strcpy(clients[POINTER_LAST_POINTER].ip, userMessage.ip);
		strncpy(clients[POINTER_LAST_POINTER].username, userMessage.username.content, sizeof(userMessage.username.content) );

		POINTER_LAST_POINTER++;

	pthread_mutex_unlock(&mutex);

	int ctrl = 1;
	while(ctrl == 1){
		read(sock, (msg *) &userMessage, sizeof(msg));

		printf("msg do read: %d \n\n\n", userMessage.operation);

		switch (userMessage.operation){
			case 1:
				//send a message
				sendToUser(&userMessage);
			break;
			case 2:
				returnUserPeerToPeer(sock, userMessage);
				//writeFile(sock);
			break;
			case 3:
				//send file
			break;
			case 4: 
				printStatusAtRightPosition(userMessage.username.content, "OFFLINE");
				printf("User %s is now disconnected \n\n", userMessage.username.content);
				close(sock);
				ctrl = -1;
			break;
			default:
				close(sock);
				ctrl = -1;
			break;
		}
	}
}


void sendToUser(msg * userMessage){
	//printf("hoiii %d\n\n\n", *clients[0].socket);
	//printf("hoiii %s \n\n", msg1);
	//printf("hoiii %s\n", userDestiny);
	serverResponse server;
	
	pthread_mutex_lock(&mutex);
	for(int i=0;i<20; i++){
		//printf("nClient %d\n", sizeof(clients[i].username));
		//printf("nMsg %d\n", sizeof(userDestiny));

		//printf("Comparacaço %d \n\n\n\n", strncmp(clients[i].username, userDestiny, strlen(clients[i].username)));
		
		if(strncmp(clients[i].username, userMessage->userDestiny.content, strlen(clients[i].username)) == 0){

			server.operation = 2; //incoming message
			strcpy(server.payload.message.content, userMessage->message.content);
			server.payload.message.nBytes = userMessage->message.nBytes;
			strcpy(server.payload.userDestiny.content, userMessage->userDestiny.content);
			server.payload.userDestiny.nBytes = userMessage->userDestiny.nBytes;




			printf("username: %s\n", userMessage->userDestiny.content);
			//printf("list: %s\n", clients[i].username);
			
			//printf("hoiii %d\n", sizeof(msg1));
			printf("username: %s \n\n", userMessage->message.content);
			write(*clients[i].socket, (char *) &server, sizeof(serverResponse));
			break;
		}
	}
	pthread_mutex_unlock(&mutex);

}


void writeFile(int sockfd) {
	FILE *fp;
	char * buffer;
	int n;
	serverResponse server;
	fileTransfer file;
	char fileDestiny[40] = {'\0'};

	strcpy(fileDestiny, "../received-");

	read(sockfd, (fileTransfer *) &file, sizeof(fileTransfer));

	printf("file name %s \n\n", file.nameFile);

    strncat(fileDestiny, file.nameFile, strlen(file.nameFile));

	printf("name File %s \n", file.nameFile);

	buffer = malloc(file.blockSize);

	printf("buffer size %lld \n", file.blockSize);

	fp = fopen(fileDestiny, "wb");

	//printf("AAAAA \n\n");
	server.operation = 0;
	while(1) {
		//printf("BBBB \n\n");
		//printf("esperando client flag %d \n\n\n", server.operation);
		read(sockfd, (serverResponse *) &server, sizeof(serverResponse));
		//printf("hhhoi %d \n\n\n", server.operation);
		//sleep(80);

		if(server.operation == -1){ //terminou o envio
			break;
		}else{
			//printf("esperando dados %d \n\n\n");
			read(sockfd, buffer, sizeof(buffer));
			//printf(" %s\n", buffer);
			//fprintf(fp, "%s", buffer);
			fwrite(buffer, 1, sizeof(buffer), fp);
			bzero(buffer, sizeof(buffer));
			
			//printf("BBBB \n\n");
		}		
		
		//printf("hhhoi \n\n\n");
	}

	printf("terminou \n\n\n");
	fclose(fp);
	return;
}


void returnUserPeerToPeer(int sock, msg userMessage){

	serverResponse server;
	
	pthread_mutex_lock(&mutex);
	for(int i=0;i<20; i++){

		if(strncmp(clients[i].username, userMessage.userDestiny.content, strlen(clients[i].username)) == 0){

			strncpy(server.payload.message.content, clients[i].ip, 15);
			server.payload.message.nBytes = 15;

			//envia de volta avisando o destino do usuario
			write(sock, (char *) &server, sizeof(serverResponse));
			

			//informa o receptor que chegara um arquivo
			server.operation = 4;
			write(*clients[i].socket, (char *) &server, sizeof(serverResponse));
			

			break;
		}
	}
	pthread_mutex_unlock(&mutex);

}