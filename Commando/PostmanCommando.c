#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#include "Pilot.h"
#include "Logger.h"
#include "PostmanCommando.h"

int sock;

extern int PostmanCommando_new(){
    struct sockaddr_in server;
    pthread_t threadListen;
     
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1){
        printf("Could not create socket");
    }
    printf("Socket created");
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( PORT );
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    printf("Connected\n");
     
    //keep communicating with server
    if(pthread_create(&threadListen, NULL, receiveMsg, NULL)){
		printf("An error occured when startint receiveMsg thread\n");
		return 1;
	}

	return 0;
}

extern void sendMsg(SocketDataSend socketData){
    if( send(sock , &socketData , strlen(socketData) , 0) < 0)
    {
        printf("Send failed");
    }
}

extern void sendMsgSetPilotState(PilotState pilotState){
    DataSend data = { 0 };
    data.pilotState = pilotState;

    SocketDataSend socketData = { 0 };
    strcpy(socketData.dataType, "setPilotState");
    socketData.data = data;
}

extern void sendMsgSetEvents(Event event){
    DataSend data = { 0 };
    data.events = events;

    SocketDataSend socketData = { 0 };
    strcpy(socketData.dataType, "SetEvents");
    socketData.data = data;
}

extern void sendMsgSetEventCount(Indice indice){
    DataSend data = { 0 };
    data.indice = indice;

    SocketDataSend socketData = { 0 };
    strcpy(socketData.dataType, "SetEventCount");
    socketData.data = data;
}

extern void *receiveMsg(void *params){
    SocketData socketData;
	for(;;){
		if( read(sock, &socketData, sizeof(socketData)) < 0)
	    {
	        puts("recv failed");
	    }else{
            if(socketData.pilotState != NULL){
                setPilotState(pilotState);
            }
            if(socketData.events != NULL){
                setEvents(events);
            }
            if(socketData.indice != NULL){
                setEventCount(indice);
            }
	    }
	}
}

extern void PostmanCommando_free(){
    close(sock);
}