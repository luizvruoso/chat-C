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

#define MAX 1024
#define PORT 8080


#define SEM_KEY		0x1243 // chave semaforo
#define SHM_KEY		0x1432 //chave memoria compartilhada

int	g_sem_id;
int	g_shm_id;
struct sockaddr_in	*g_shm_addr;

struct sembuf	g_sem_op1[1];
struct sembuf	g_sem_op2[1];


#define SA struct sockaddr

// Function designed for chat between client and server.
//void func(int sockfd, int, int);

void registerUser(int,  int ,struct sockaddr_in * );
// Driver function
int main(){
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	g_sem_op1[0].sem_num   =  0;
	g_sem_op1[0].sem_op    = -1;
	g_sem_op1[0].sem_flg   =  0;

	g_sem_op2[0].sem_num =  0;
	g_sem_op2[0].sem_op  =  1;
	g_sem_op2[0].sem_flg =  0;

	if( ( g_sem_id = semget( SEM_KEY, 1, IPC_CREAT | 0666 ) ) == -1 ) {
		fprintf(stderr,"chamada a semget() falhou, impossivel criar o conjunto de semaforos!");
		exit(1);
	}
	
	if( semop( g_sem_id, g_sem_op2, 1 ) == -1 ) {
		fprintf(stderr,"chamada semop() falhou, impossivel inicializar o semaforo!");
		exit(1);
	}
	/*
	 * Criando o segmento de memoria compartilhada
	 */
	if( (g_shm_id = shmget( SHM_KEY, sizeof(int), IPC_CREAT | 0666)) == -1 ) {
		fprintf(stderr,"Impossivel criar o segmento de memoria compartilhada!\n");
		exit(1);
	}
	if( (g_shm_addr = (int *)shmat(g_shm_id, NULL, 0)) == (int *)-1 ) {
		fprintf(stderr,"Impossivel associar o segmento de memoria compartilhada!\n");
		exit(1);
	}
	//*g_shm_addr = NULL;


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
	for(int i = 0; i < 2; i++){

		printf("port %d \n\n", cli.sin_port);

		connfd = accept(sockfd, (SA*)&cli, &len);
		if (connfd < 0) {
			printf("server acccept failed...\n");
			exit(0);
		}
		else
			printf("server accept the client...\n");

		if(fork() == 0){
			//bloqueando semafaro
			if( semop( g_sem_id, g_sem_op1, 1 ) == -1 ) {
				fprintf(stderr,"chamada semop() falhou, impossivel fechar o recurso!");
				exit(1);
			}
			//fim	
			//*g_shm_addr = connfd;
			g_shm_addr->sin_addr = cli.sin_addr;
			g_shm_addr->sin_family = cli.sin_family;
			g_shm_addr->sin_port = cli.sin_port;
			//g_shm_addr->sin_zero = cli.sin_zero;

			if( semop( g_sem_id, g_sem_op2, 1 ) == -1 ) {      		
				fprintf(stderr,"chamada semop() falhou, impossivel liberar o recurso!");
				exit(1);
			}


			printf("port %d \n\n", cli.sin_port);
			// Function for chatting between client and server
			//printf("\nDescritores:  sock: %d    msgsock:%d \n\n", sockfd, connfd);
			registerUser(sockfd, connfd, &cli);
				// After chatting close the socket
			close(sockfd);
		}
	}

	for(int i = 0; i < 2; i++){
		wait(NULL);
	}
	 /*
	* Removendo a memoria compartilhada
	*/
	if( shmctl(g_shm_id,IPC_RMID,NULL) != 0 ) {
		fprintf(stderr,"Impossivel remover o segmento de memoria compartilhada!\n");
		exit(1);
	}

	/*
	* Removendo o semaforo
	*/
	if( semctl( g_sem_id, 0, IPC_RMID, 0) != 0 ) {
		fprintf(stderr,"Impossivel remover o conjunto de semaforos!\n");
		exit(1);
	}

	exit(0);
	
}


void registerUser(int sockfd, int connfd, struct sockaddr_in * client)
{
	char buff[MAX];
	int n;
	serverResponse message;
	msg userMessage;
	FILE *file;
	int sock;

	read(connfd, (char *) &userMessage, sizeof(userMessage));
	message.statusCode = 1;
	strcpy(message.message, "YES u can!");
	
	if(searchUser(userMessage.username) == 0){
		//usuario ja registrado, fica online
		printStatusAtRightPosition(userMessage.username, "ONLINE");
	}else{
		//novo usuario é registrado e fica online
		printAtEndOfFile(userMessage.username, "ONLINE");
	}

	
	write(connfd, (char *) &message, sizeof(message));

	//printf("hoooixinho darlyn %d \n", *g_shm_addr);
	while(1) {
		bzero(buff, MAX);

		// read the message from client and copy it in buffer
		read(connfd, buff, sizeof(buff));
		printf("sockfd: %d \n\n", connfd);
		// print buffer which contains the client contents
		printf("\nFrom client: %s\t To client : ", buff);
		printf("\n");
		bzero(buff, MAX);
		n = 0;
		// copy server message in the buffer
		
		while ((buff[n++] = getchar()) != '\n');
		
		
		// and send that buffer to client
		//if(sendto(sock, (char *) buff, sizeof(buff), 0, (struct sockaddr *) g_shm_addr, sizeof g_shm_addr) < 0) 
         //  perror("Envio da mensagem"); 
      
		write(connfd, buff, sizeof(buff));


		// if msg contains "Exit" then server exit and chat ended.
		if (strncmp("exit", buff, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}

		exit(0);
	}
}
