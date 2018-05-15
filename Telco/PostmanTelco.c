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

#include "ProxyPilot.h"
#include "ProxyLogger.h"
#include "PostmanTelco.h"

int sock;

extern int PostmanTelco_new(){
	int socket_ecoute;
	struct sockaddr_in mon_adresse;

	socket_ecoute = socket(AF_INET, SOCK_STREAM, 0);
	mon_adresse.sin_family = AF_INET;
	mon_adresse.sin_port = htons (PORT);
	mon_adresse.sin_addr.s_addr = htonl (INADDR_ANY);

	bind(socket_ecoute, (struct sockaddr *)&mon_adresse, sizeof (mon_adresse));

	listen(socket_ecoute, MAX_PENDING_CONNECTIONS);
	
	pthread_t threadListen;
	while(1){
		sock = accept(socket_ecoute, NULL, 0);
		printf("New client !");
		// thread reading socket
		if(pthread_create(&threadListen, NULL, receiveMsg, NULL)){
			printf("An error occured when startint receiveMsg thread\n");
		}
	}
	return 0;
}

extern void sendMsg(SocketData socketData){
	write(sock, &socketData, sizeof(socketData));
}

extern void sendMsgAskEvent(Indice from, Indice to){
	Data data = { 0 };
	data.fromIndice = from;
	data.toIndice = to;

	SocketData socketData = { 0 };
	strcpy(socketData.dataType, "AskEvent");
	socketData.data = data;

	sendMsg(socketData);
}

extern void sendMsgAskEventCount(){
	Data data = { 0 };

	SocketData socketData = { 0 };
	strcpy(socketData.dataType, "AskEventCount");

	sendMsg(socketData);
}

extern void sendMsgSetRobotVelocity(Velocity vel){
	Data data = { 0 };
	data.velocityVector = vel;

	SocketData socketData = { 0 };
	strcpy(socketData.dataType, "SetRobotVelocity");
	socketData.data = data;

	sendMsg(socketData);
}

extern void sendMsgAskPilotState(){
	Data data = { 0 };

	SocketData socketData = { 0 };
	strcpy(socketData.dataType, "AskPilotState");

	sendMsg(socketData);
}

extern void sendMsgToggleEmergencyStop(){
	Data data = { 0 };

	SocketData socketData = { 0 };
	strcpy(socketData.dataType, "ToggleEmergencyStop");

	sendMsg(socketData);
}

extern void *receiveMsg(void *param){
    SocketData socketData;
	for(;;){
		if( read(sock, &socketData, sizeof(socketData)) < 0)
	    {
	        puts("recv failed");
	    }else{
            // if(socketData.pilotState != NULL){
            //     setPilotState(pilotState);
            // }
            // if(socketData.events != NULL){
            //     setEvents(events);
            // }
            // if(socketData.indice != NULL){
            //     setEventCount(indice);
            // }
	    }
	}
}

extern void PostmanTelco_free(){
	close(sock);
}