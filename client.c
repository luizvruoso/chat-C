#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "lib/structs.h"
#include "lib/filesOps.h"
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/ipc.h>   
#include <sys/ioctl.h>
#include <unistd.h>

#define MAX 1024
#define PORT 2020
#define PORT_TRANSFER 9091
#define PORT_SEND 1236

#define SA struct sockaddr
pthread_mutex_t mutex;
pthread_mutex_t ScanMutex;
messageReceived messages[100];
int lastPositionMessage = 0;


void messageHandler(int * , char *);
void * recvMsg(void * sockfd);
void sendFile(int , msg ,char * );
void prepareScopeToSendFile(int, msg);
void * fileReceive(void * );
void writeFile(int );
int main(){
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;
 	struct ifreq ifr;

	//socket create and varification
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
	//servaddr.sin_addr.s_addr = inet_addr("192.168.0.115");
	servaddr.sin_addr.s_addr =  htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);
	

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name , "eth0" , IFNAMSIZ-1);

	ioctl(sockfd, SIOCGIFADDR, &ifr);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server!\n");
	
	// function for chat
	messageHandler(&sockfd, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	// close the socket
	close(sockfd);
}

void messageHandler(int *sockfd, char * localIP){
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

	strncpy(userMessage.username.content, buff, n);
	userMessage.username.nBytes = n;

	strcpy(userMessage.ip, localIP);

	write(*sockfd, (char *) &userMessage, sizeof(msg));

	//server response:
	read(*sockfd, (serverResponse *) &serverResp, sizeof(serverResponse));


	if(serverResp.operation == 1 || serverResp.operation == 6){
		if(serverResp.operation == 6){
			printf("\n\nYou have unread messages\n\n");
			while(1){
				read(*sockfd, (serverResponse *) &serverResp, sizeof(serverResponse));

				if(serverResp.operation == -1) break;		
				else{
					printf("\n Message: %s", serverResp.payload.message.content);
				}
			}
		}else{
			printf("\n\nServer answer: %s", serverResp.payload.message.content);
		}
	}else{
		printf("\n Server not responding ...\n");
		close(*sockfd);
		exit(0);
	}

	//THREAD TO RECeIVE INCOMING MESSAGES
	pthread_create(&thread_id, NULL, (void *)recvMsg, sockfd);
	//END
	int ctrl = 1;
	int i = 0;
	while(ctrl == 1) {
		menu(&op);
		switch(op){
			case 1: 
				sendUserMessage(*sockfd, userMessage);
			break;
			case 2:
				prepareScopeToSendFile(*sockfd, userMessage);
			break;
			case 3:
				pthread_mutex_lock(&mutex);
				while(1){
						if(lastPositionMessage == i){
							lastPositionMessage = 0;
							i = 0;
							break;
						};
						printf("\nReceived message from: %s \n", messages[i].from);
						printf("\nMessage: %s \n", messages[i].message);
					i++;
				}
				pthread_mutex_unlock(&mutex);


			break;
			case 4:
				userMessage.operation = 4;
				write(*sockfd, (char * ) &userMessage, sizeof(msg));
				close(*sockfd);
				ctrl = -1;
			break;
		}		
	}
}

void prepareScopeToSendFile(int sock, msg userMessage){
	char buff[1024];
	int n;
	serverResponse server;

	printf("Enter the target user: \n");
	n = 0;
	while ((buff[n++] = getchar()) != '\n');
	n--;

	bzero(userMessage.userDestiny.content, sizeof(userMessage.userDestiny.nBytes));
	strncpy(userMessage.userDestiny.content, buff, n);

	userMessage.operation = 2;
	write(sock, (char * ) &userMessage, sizeof(msg));

	read(sock, (serverResponse *) &server, sizeof(server));

	sendFile(sock, userMessage, server.payload.message.content);
}

void sendFile(int sock, msg userMessage, char * ip){
	serverResponse server;
	char data[MAX] = {0};
	FILE *filePointer;
	long long size = 0;
	char buff[40] = {'\0'};
	char buffFile[40] = {'\0'};
	long long vectorSize = 0;
	fileTransfer file;
	struct sockaddr_in servaddr;
	char localIP[15];
	int sock2;
	strncpy(localIP, ip, 15);


	//close(sock);

	sock2 = socket(AF_INET, SOCK_STREAM, 0);

	if (sock2 == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	servaddr.sin_family = AF_INET;
	
	//servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_addr.s_addr = inet_addr(localIP);
	servaddr.sin_port = htons(PORT_TRANSFER);
	
	printf("%s \n", localIP);
	sleep(2);
	// connect the client socket to server socket
	if (connect(sock2, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server!\n");


	printf("connected %d \n\n", sock2);

	sock = sock2;


	int n;

	printf("Enter the name of the file you want to send: \n");
	n = 0;
	while ((buff[n++] = getchar()) != '\n');
	n--;

	//getchar();
	printf("File name %s", buff);


	strncpy(buffFile, buff, n);
	filePointer = fopen(buffFile, "rb");

	fseek(filePointer, 0, SEEK_END);
	size = ftell(filePointer);
	fseek(filePointer, 0, SEEK_SET);

	//printf("tamanho arquivo %lld \n\n\n", size);
	//return;
	//fgets(server.payload.message.content, sizeof(server.payload.message.content), filePointer);
	//printf("Value da linha %s \n\n\n", server.payload.message.content);

	//sleep(80);
	/*
	if(size <= 10240){
		vectorSize = size;
	}else{
		vectorSize = 1048576;
	}*/
	vectorSize = 1024;



	char * buffer;
	buffer = malloc(vectorSize);

	strncpy(file.nameFile, buff, strlen(buff));
	
	file.blockSize = vectorSize;	
	printf("block size %lld \n\n", file.blockSize);

	char loading[100] = {'\0'};
	//int aux = (int) (vectorSize * 10)/size;
	//printf("size %d \n\n", aux);
	//loading = malloc(aux);

	//sleep(50);

	write(sock, (char *) &file, sizeof(fileTransfer));

	//printf("size %d \n\n", aux);

	//server.operation = 2;
	int k = 1;
	int countLoading = 1;
	int sizeAux = 0;
	size = 1024;
	size_t result;
	do{
		
		//fgets(buffer, sizeof(buffer), filePointer);
		server.operation = 2;
		//printf("loop \n\n");
		//printf("get\n\n");
		if (write(sock, (char *) &server, sizeof(serverResponse)) == -1) {
			perror("error in sending data");
			break;
			//exit(1);
		}
		
		result = fread(buffer, 1, 1024, filePointer);
		//printf("result %d \n \n", sizeof(buffer));

		
		

		write(sock, buffer, 1024);
		
		//printf("enviou\n\n");
		bzero(buffer, sizeof(buffer));

		//printf("result %d \n\n", result);
		//printf("k %d \n\n",  k);
		//printf("size %d \n\n", size);

		if(( (size/1024)/10) <= (k*result) ){
			
			strncat(loading, "#", 1);
			printf("\r %s", loading);
			fflush(stdout);
			
			//sleep(80);
			countLoading ++;
			k = 0;
			
		}

		k++;


		
		


	}while(!feof(filePointer));

	printf("\n File transferred \n\n");

	fclose(filePointer);
	server.operation = -1;
	
	write(sock, (char *) &server, sizeof(serverResponse));
	sleep(2);
	close(sock);
	return ;



}

void sendUserMessage(int sock, msg userMessage){
char buff[MAX]={0};
		char buffUser[MAX] = {0};
		char buffSpecificUser[MAX] = {0};
		int n;
		int i = 0;
		int j = 0;
		int nUser;

		//send message to
		bzero(buffUser, sizeof(buffUser));
		printf("\nEnter the user : ");
		nUser = 0;
		while ((buffUser[nUser++] = getchar()) != '\n');
		nUser--;
		// bzero(userMessage.userDestiny.content, 1024);
		// strncpy(userMessage.userDestiny.content, buffUser, nUser);	
		// userMessage.userDestiny.nBytes = nUser;
		
		
		// userMessage.operation = 1;// send file operation
		//end
		
		//message to be sended
		// bzero(buff, sizeof(buff));
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

		while(i <= nUser) {
			if(buffUser[i] == ' ' || i == nUser) {
				bzero(userMessage.userDestiny.content, 1024);
				userMessage.userDestiny.nBytes = 0;
				strncpy(userMessage.userDestiny.content, buffSpecificUser, j);	
				userMessage.userDestiny.nBytes = j;
				userMessage.operation = 1;
				i++;
				j = 0;
				bzero(buffSpecificUser, sizeof(buffSpecificUser));

				write(sock, (char *) &userMessage, sizeof(msg));
				sleep(2);
			} else {
				buffSpecificUser[j] = buffUser[i];
				i++;
				j++;
			}
		}

		// write(sock, (char *) &userMessage, sizeof(msg));
		// sleep(2);
		// envio 
}

void menu(int * op){
	printf("\n1 - Send Message \n");
	printf("2 - Send File \n");

	pthread_mutex_lock(&mutex);

	if(lastPositionMessage != 0){
		printf("3 - Messages - You have %d unread \n", lastPositionMessage);
	}else{
		printf("3 - Messages \n");
	}

	pthread_mutex_unlock(&mutex);

	printf("4 - Exit \n");
	scanf("%d", op);
	getchar();
}

void * recvMsg(void * sockfd){
	int sock = *((int *)sockfd);
	int len;
	msg msgFromServer;
	serverResponse server;
	pthread_t thread_id;

	int ctrl = 1;
	while(ctrl == 1){
		//printf("read \n");
		read(sock, (serverResponse *) &server, sizeof(serverResponse));
		
		switch(server.operation){
			case 1:
				printf("\t\n Server close connection. Exiting..... \n");	
				close(sock);
				exit(0);
			break;
			case 2:
				pthread_mutex_lock(&mutex);
				printf("\n Alert: %d messages unread \n ", lastPositionMessage + 1);
				strcpy(messages[lastPositionMessage].message, server.payload.message.content);
				strcpy(messages[lastPositionMessage].from, server.payload.userDestiny.content);		
				lastPositionMessage++;
				pthread_mutex_unlock(&mutex);

			break;
			case 3: //CONTACT LIST


			break;
			case 4: //INCOMING FILE
				//THREAD TO RECIVE INCOMING MESSAGES
				pthread_create(&thread_id, NULL, (void *)fileReceive, &sock);
				//END
			break;
			default:
				printf("\t\n Server close connection. Exiting..... \n");	
				close(sock);
				ctrl = -1;
			break;
		}
	}
}

void * fileReceive(void * sockfd){
	struct sockaddr_in cli, servaddr;
	int sock = *((int *)sockfd);
	int connfd;
	int sock2;
	int len;

	sock2 = socket(AF_INET, SOCK_STREAM, 0);

	if (sock2 == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT_TRANSFER);
 
	// Binding newly created socket to given IP and verification
	if ((bind(sock2, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		//printf("Socket successfully binded..\n");
	

	if ((listen(sock2, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		//printf("Server listening..\n");

	len = sizeof(cli);

	//printf("\n receiving file.... \n");
	connfd = accept(sock2, (SA*) &cli, &len);

	if (connfd < 0) {
		printf("server acccept failed...\n");
		exit(0);
	}
	else
		//printf("server accept the client...\n");

	writeFile(connfd);

	close(connfd);

	
	pthread_exit(pthread_self());

	
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

	//printf("File name %s \n\n", file.nameFile);

    strncat(fileDestiny, file.nameFile, strlen(file.nameFile)-1);

	//printf("Name File %s \n", file.nameFile);

	buffer = malloc(file.blockSize);

	//printf("Buffer size %lld \n", file.blockSize);

	fp = fopen(fileDestiny, "wb");

	server.operation = 0;
	while(1) {

		read(sockfd, (serverResponse *) &server, sizeof(serverResponse));


		if(server.operation == -1){ //terminou o envio
			break;
		}else{
			read(sockfd, buffer, 1024);
			fwrite(buffer, 1, 1024, fp);
			bzero(buffer, sizeof(buffer));
		}	

	}
	fclose(fp);

	printf("\nAlert: You received a new file. Type 'n' to continue...\n");
	printf("File Name: ../received-%s \n", file.nameFile);

	//printf("\n File Received \n");

	return;
}

