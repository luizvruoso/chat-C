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
#include <sys/shm.h>            /* for shmget(), shmat(), shmctl() */
#include <sys/sem.h>            /* for semget(), semop(), semctl() */
#include <pthread.h>

#define MAX 1024
#define PORT 8080
#define SA struct sockaddr


pthread_mutex_t mutex;
int clients[20];

// Function designed for chat between client and server.
//void func(int sockfd, int, int);

void registerUser(int);
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
	for(int i = 0; i < 2; i++) {

		printf("port %d \n\n", cli.sin_port);

		connfd = accept(sockfd, (SA*)&cli, &len);
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
		clients[i] = connfd;
		// Function for chatting between client and server
		//printf("\nDescritores:  sock: %d    msgsock:%d \n\n", sockfd, connfd);
		pthread_create(&thread_id, NULL, (void *)registerUser, &connfd);
		//registerUser(sockfd, connfd, &cli);
		pthread_mutex_unlock(&mutex);
		// After chatting close the socket
		
	}
	for(int i = 0; i < 2; i++) {
		wait(NULL);
	}
	close(sockfd);
	
	exit(0);
}

void registerUser(int sockfd)
{
	char buff[MAX];
	int n;
	serverResponse message;
	msg userMessage;
	FILE *file;
	int sock;

	read(sockfd, (char *) &userMessage, sizeof(userMessage));
	message.statusCode = 1;
	strcpy(message.message, "YES u can!");
	
	if(searchUser(userMessage.username) == 0) {
		//usuario ja registrado, fica online
		printStatusAtRightPosition(userMessage.username, "ONLINE");
	}else {
		//novo usuario é registrado e fica online
		printAtEndOfFile(userMessage.username, "ONLINE");
	}
	
	write(sockfd, (char *) &message, sizeof(message));

	//printf("hoooixinho darlyn %d \n", *g_shm_addr);
	while(1) {
		bzero(buff, MAX);

		// read the message from client and copy it in buffer
		read(sockfd, buff, sizeof(buff));
		printf("sockfd: %d \n\n", sockfd);
		// print buffer which contains the client contents
		printf("\nFrom client: %s\t To client : ", buff);
		printf("\n");
		bzero(buff, MAX);
		n = 0;
		// copy server message in the buffer
		
		while ((buff[n++] = getchar()) != '\n');
		//ow
		//e esse while aqui em cima?
		
		// and send that buffer to client
		//if(sendto(sock, (char *) buff, sizeof(buff), 0, (struct sockaddr *) g_shm_addr, sizeof g_shm_addr) < 0) 
       	 	//perror("Envio da mensagem"); 
      
		write(sockfd, buff, sizeof(buff));

		// if msg contains "Exit" then server exit and chat ends.
		if (strncmp("exit", buff, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}

		exit(0);
	}
}
