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

extern void sendMsg(SocketDataSend socketData){
	write(sock, &socketData, sizeof(socketData));
}

extern void sendMsgAskEvent(Indice from, Indice to){
	DataSend data;
	data.fromIndice = from;
	data.toIndice = to;

	SocketDataSend socketData;
	socketData.dataType = ASK_EVENT;
	socketData.data = data;

	sendMsg(socketData);
}

extern void sendMsgAskEventCount(){
	SocketDataSend socketData;
	socketData.dataType = ASK_EVENT_COUNT;

	sendMsg(socketData);
}

extern void sendMsgSetRobotVelocity(VelocityVector vel){
	DataSend data;
	data.velocityVector = vel;

	SocketDataSend socketData;
	socketData.dataType = SET_ROBOT_VELOCITY;
	socketData.data = data;

	sendMsg(socketData);
}

extern void sendMsgAskPilotState(){
	SocketDataSend socketData;
	socketData.dataType = ASK_PILOT_STATE;

	sendMsg(socketData);
}

extern void sendMsgToggleEmergencyStop(){
	SocketDataSend socketData;
	socketData.dataType = TOGGLE_EMERGENCY_STOP;

	sendMsg(socketData);
}

extern void *receiveMsg(void *param){
    SocketDataReceive socketData;
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