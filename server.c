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
#define PORT 1234
#define SA struct sockaddr
#define MAXUSERS 20

int POINTER_LAST_POINTER = 0;

pthread_mutex_t mutex;
user clients[50];

// Function designed for chat between client and server.
//void func(int sockfd, int, int);

void * registerUser(void *);
void sendToUser(char * , char * );

// Driver function
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

		//printf("port %d \n\n", cli.sin_port);

		connfd = accept(sockfd, (SA*) &cli, &len);
		if (connfd < 0) {
			printf("server acccept failed...\n");
			exit(0);
		}
		else
			printf("server accept the client...\n");
		

		//pthread_create(&thread_id, NULL, myThreadFun, NULL);
		//pthread_join(thread_id, NULL);

		pthread_mutex_lock(&mutex);
		//printf("port %d \n\n", cli.sin_port);
		
		//clients[i] = connfd;

		// Function for chatting between client and server
		//printf("\nDescritores:  sock: %d    msgsock:%d \n\n", sockfd, connfd);
		pthread_create(&thread_id, NULL, (void *)registerUser, &connfd);
		//registerUser(sockfd, connfd, &cli);
		pthread_mutex_unlock(&mutex);
		// After chatting close the socket
		
	}
	
	return 0;
	//close(sockfd);
	//exit(0);
}

void * registerUser(void * sockfd)
{
	char buff[MAX];
	int n;
	serverResponse message;
	msg userMessage;
	int sock = *((int *) sockfd);


	//bzero(userMessage.username, sizeof(userMessage.username));
	//bzero(userMessage.userDestiny, sizeof(userMessage.userDestiny));
	bzero(userMessage.message, sizeof(userMessage.message));

	read(sock, (struct msg *) &userMessage, sizeof(userMessage));

	message.statusCode = 1;
	strcpy(message.message, "YES u can!");
	
	write(sock, (char *) &message, sizeof(serverResponse));

	if(searchUser(userMessage.username) == 0) {
		//usuario ja registrado, fica online
		printStatusAtRightPosition(userMessage.username, "ONLINE");
	}else {
		//novo usuario é registrado e fica online
		printAtEndOfFile(userMessage.username, "ONLINE");
	}
	pthread_mutex_lock(&mutex);
	
		clients[POINTER_LAST_POINTER].socket = &sock;
		//printf("socket no role: %d \n\n\n", *clients[0].socket);
		
		//write(sock, message.message, sizeof(message.message));
		
		//printf("socket no role: %d \n\n\n", *clients[0].socket);
		strncpy(clients[POINTER_LAST_POINTER].username, userMessage.username, sizeof(userMessage.username) );
		//strcpy(clients[POINTER_LAST_POINTER].username, userMessage.username);
		POINTER_LAST_POINTER++;

	pthread_mutex_unlock(&mutex);


	//strcmp(buff, "fafassadsadsa");

	//write(sock, (char *) &message, sizeof(message));
	while(1){
		//memcpy(userMessage.message,'\0', sizeof(userMessage.message));
		//strncpy(userMessage.message, '\0', 1023);
		read(sock, (char *) &userMessage, sizeof(msg));
		printf("msg do read: %s \n\n\n", userMessage.message);
		if(strncmp(userMessage.message, "exit", 4) == 0){
			break;
		}else{
			sendToUser(userMessage.message, userMessage.userDestiny);
		}

	}

	//printf("hoooixinho darlyn %d \n", *g_shm_addr);

	/*
	while(1) {
		bzero(buff, MAX);

		// read the message from client and copy it in buffer
		read(sock, buff, sizeof(buff));
		printf("sockfd: %d \n\n", sockfd);
		// print buffer which contains the client contents
		printf("\nFrom client: %s\t To client : ", buff);
		printf("\n");
		bzero(buff, MAX);
		n = 0;
		// copy server message in the buffer
		
		while ((buff[n++] = getchar()) != '\n');
		
		// and send that buffer to client
		//if(sendto(sock, (char *) buff, sizeof(buff), 0, (struct sockaddr *) g_shm_addr, sizeof g_shm_addr) < 0) 
       	 	//perror("Envio da mensagem");
      
		write(sock, buff, sizeof(buff));

		// if msg contains "Exit" then server exit and chat ends.
		if (strncmp("exit", buff, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}
		

		//exit(0);
	}*/
}


void sendToUser(char * msg1, char * userDestiny){
	//printf("hoiii %d\n\n\n", *clients[0].socket);
	//printf("hoiii %s \n\n", msg1);
	//printf("hoiii %s\n", userDestiny);
	

	pthread_mutex_lock(&mutex);
	for(int i=0;i<20; i++){
		//printf("nClient %d\n", sizeof(clients[i].username));
		//printf("nMsg %d\n", sizeof(userDestiny));

		//printf("Comparacaço %d \n\n\n\n", strncmp(clients[i].username, userDestiny, strlen(clients[i].username)));
		
		if(strncmp(clients[i].username, userDestiny, strlen(clients[i].username)) == 0){

			printf("usernmae %s\n", userDestiny);
			//printf("list: %s\n", clients[i].username);
			
			printf("hoiii %d\n", sizeof(msg1));
			write(*clients[i].socket, msg1, 1023);
			break;
		}
	}
	pthread_mutex_unlock(&mutex);

	
	


	


}
