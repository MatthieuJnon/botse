#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#include "PostmanTelco.h"

int *cli_sockfd;
int client_id;
int clientToFill;

int PostmanTelco_new(){
	int sockfd;
	struct sockaddr_in addr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("ERROR opening socket\n");
		return 1;
	}
	// set the port reusable when closed
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
	    printf("setsockopt(SO_REUSEADDR) failed");

	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
	addr.sin_addr.s_addr = htonl (INADDR_ANY);

	if(bind(sockfd, (struct sockaddr *)&addr, sizeof (addr)) < 0){
		printf("CANNOT BIND\n");
		return 1;
	}

	if(listen(sockfd, MAX_PENDING_CONNECTIONS) < 0){
		printf("CANNOT LISTEN\n");
		return 1;
	}

	pthread_t threadListen;

	// list of clients socket
	int newClient;
	client_id = 0;
	cli_sockfd = (int*)malloc(MAX_PENDING_CONNECTIONS*sizeof(int)); /* nb clients sockets */
	memset(cli_sockfd, 0, MAX_PENDING_CONNECTIONS*sizeof(int));

	while(1){
		socklen_t clilen;
		struct sockaddr_in cli_addr;

		printf("-> Listening for clients...\n");

		/* Zero out memory for the client information. */
		memset(&cli_addr, 0, sizeof(cli_addr));

		clilen = sizeof(cli_addr);

		/* Accept the connection from the client (temporarily) */
		newClient = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		while(cli_sockfd[client_id] != 0){
			client_id++;
			if(client_id >= MAX_PENDING_CONNECTIONS){
				client_id = 0;
				break;
			}
		}

		if(cli_sockfd[client_id] == 0){	// we recheck if the selected client is free
			cli_sockfd[client_id] = newClient;

			if (cli_sockfd[client_id] < 0)
			    printf("ERROR accepting a connection from a client.");

			printf("New client : id is %d\n", client_id);

			/* Send the client it's ID. */
			// char helloMsg[] = "Hello, you are client %d";
			// sprintf(helloMsg, helloMsg, client_id);
			// sendMsg(cli_sockfd[client_id], helloMsg);

			// thread reading socket
			if(pthread_create(&threadListen, NULL, receiveMsg, &cli_sockfd[client_id])){
				printf("An error occured when startint receiveMsg thread\n");
			}
		}else{
			printf("Client %d is already taken\n", client_id);
			close(newClient);
		}
	}
	return 0;
}

extern void sendMsg(int socket, char *msg){
	if(msg != NULL && strlen(msg) > 0 && socket != 0){	// only to connected clients
		if(strchr(msg, '\n') == NULL){
			strcat(msg, "\n");
		}
		write(socket, msg, strlen(msg));
	}
}

extern void *receiveMsg(void *param){
	int *socket = (int *)param;
	char msg[2000];
	for(;;){
		// reinitialize msg
		memset(msg, 0, 255);
		int n = read(*socket, msg, 255);

		if (n <= 0){
			printf("Client disconnected\n----------------------------\n");
			close(*socket);
			*socket = 0;
			pthread_exit(NULL);
		}else if(strchr(msg, '\n') != NULL){
			printf("Client sent : %s\n", msg);
		}
	}
}

void PostmanTelco_free(){

}