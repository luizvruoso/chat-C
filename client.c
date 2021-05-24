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
#include <sys/ipc.h>            /* for all IPC function calls */
#include <sys/ioctl.h>
#include <unistd.h>

#define MAX 1024
#define PORT 2020
#define PORT_TRANSFER 9090
#define PORT_SEND 1236

#define SA struct sockaddr
pthread_mutex_t mutex;
pthread_mutex_t ScanMutex;


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
	servaddr.sin_addr.s_addr = inet_addr("192.168.0.115");
	//servaddr.sin_addr.s_addr =  htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);
	

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name , "eth0" , IFNAMSIZ-1);

	ioctl(sockfd, SIOCGIFADDR, &ifr);

 	//printf("dasdsadsa %s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));


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
				prepareScopeToSendFile(*sockfd, userMessage);
				//sendFile(*sockfd, userMessage);
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


	//sending intention to server
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

	printf("tamanho arquivo %lld \n\n\n", size);
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
		printf("result %d \n \n", sizeof(buffer));

		
		

		write(sock, buffer, 1024);
		
		//printf("enviou\n\n");
		bzero(buffer, sizeof(buffer));

		//printf("result %d \n\n", result);
		//printf("k %d \n\n",  k);
		//printf("size %d \n\n", size);

		if(( (size/result)/10) <= (k*result) ){
			
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
		
		
		userMessage.operation = 1;// send file operation
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
		sleep(2);
		// envio 
}


void menu(int * op){

	printf("\n1 - Send Message \n");
	printf("2 - Send File \n");
	printf("3 - Mensagens \n");
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

	//printw("%d \n", MAX_LINES);
	while(1){
		printf("read \n");
		read(sock, (serverResponse *) &server, sizeof(serverResponse));
		
		switch(server.operation){
			case 1:
				printf("\t \nServer close connection. Exiting..... \n");	
				close(sock);
				exit(0);

			break;

			case 2:
				//pthread_mutex_lock(&ScanMutex);
				fflush(stdin);
				printf("\r Recieved message from: %s \n", server.payload.userDestiny.content);	
				printf("\r Message: %s \n", server.payload.message.content);	
				fflush(stdin);
				//pthread_mutex_unlock(&ScanMutex);
				//sleep(2);
			break;

			case 3: //CONTACT LIST


			break;

			case 4: //INCOMING FILE
				
				//THREAD TO RECIVE INCOMING MESSAGES
				pthread_create(&thread_id, NULL, (void *)fileReceive, &sock);
				//END
				

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
	//close(sock);

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
		printf("socket bind failed...1\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");
	

	if ((listen(sock2, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");

	len = sizeof(cli);

	printf("\n receiving file.... \n");
	connfd = accept(sock2, (SA*) &cli, &len);

	if (connfd < 0) {
		printf("server acccept failed...\n");
		exit(0);
	}
	else
		printf("server accept the client...\n");

	//printf("%d \n\n", connfd);
	writeFile(connfd);

	close(connfd);
	//close(sock);

	printf("Type something to continue...\n");
	
	pthread_exit(pthread_self());

	
}

void writeFile(int sockfd) {
	FILE *fp;
	char * buffer;
	int n;
	serverResponse server;
	fileTransfer file;
	char fileDestiny[40] = {'\0'};
	
	//printf("%d \n\n", sockfd);

	//sleep(5);

	strcpy(fileDestiny, "../received-");
	//sleep(10);
	read(sockfd, (fileTransfer *) &file, sizeof(fileTransfer));

	printf("file name %s \n\n", file.nameFile);

    strncat(fileDestiny, file.nameFile, strlen(file.nameFile)-1);

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
			read(sockfd, buffer, 1024);
			//printf(" %s\n", buffer);
			//fprintf(fp, "%s", buffer);
			fwrite(buffer, 1, 1024, fp);
			bzero(buffer, sizeof(buffer));
			
			//printf("BBBB \n\n");
		}	
		//usleep(15);
		
		//printf("hhhoi \n\n\n");
	}
	fclose(fp);
	printf("\nterminou \n\n\n");
	//fflush(stdin);

	return;
}

